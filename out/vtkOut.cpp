/// \file vtkOut.cpp
/// \brief solution output in VTK XML format
/// \author LNM RWTH Aachen: Jens Berger, Patrick Esser, Joerg Grande, Sven Gross, Martin Horsky, Volker Reichelt; SC RWTH Aachen: Oliver Fortmeier

/*
 * This file is part of DROPS.
 *
 * DROPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DROPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DROPS. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Copyright 2009 LNM/SC RWTH Aachen, Germany
*/

#include "out/vtkOut.h"

namespace DROPS
{

VTKOutCL::VTKOutCL(const MultiGridCL& mg, const std::string& dataname, Uint numsteps,
            const std::string& dirname, const std::string& filename, bool binary, bool onlyP1, Uint lvl)
/** Beside constructing the VTKOutCL, this function computes the number of
    digits, that are used to decode the time steps in the filename.
\param mg        MultiGridCL that contains the geometry
\param dataname  name of the data
\param numsteps  number of time steps
\param filename  prefix of all files (e.g. vtk/output)
\param binary    Write out files in binary format.
\param lvl       Multigrid level
*/
    : mg_(mg), timestep_(0), numsteps_(numsteps), descstr_(dataname),
        dirname_(dirname), filename_(filename), binary_(binary), onlyP1_(onlyP1), geomwritten_(false),
        vAddrMap_(), eAddrMap_(), coords_(), tetras_(), lvl_(lvl),
        numPoints_(0), numTetras_(0)
{
    if (!dirname.empty() && *dirname.rbegin()!='/' )
        dirname_+= '/';
    decDigits_= 1;
    while( numsteps>9){ ++decDigits_; numsteps/=10; }
}

VTKOutCL::~VTKOutCL ()
{
    for (std::map<std::string,VTKVariableCL*>::iterator it= vars_.begin(); it != vars_.end(); ++it)
        delete it->second;
}

void VTKOutCL::Register (VTKVariableCL& var)
{
    if( vars_.find(var.varName()) != vars_.end())
        std::cout << "Error! Variable name is used twice! No registration of the variable is carried out!" << std::endl;
    else
        vars_[var.varName()]= &var;
}

void VTKOutCL::Write ( double time, bool writeDistribution)
{
    PutGeom( time, writeDistribution);
    for( std::map<std::string, VTKVariableCL*>::iterator it= vars_.begin(); it != vars_.end(); ++it) {
        it->second->put( *this);
    }
    Commit();
}

void VTKOutCL::AppendTimecode( std::string& str) const
/** Appends a time-code to the filename*/
{
    char format[]= "%0Xi",
         postfix[8];
    format[2]= '0' + char(decDigits_);
    std::sprintf( postfix, format, timestep_);
    str+= postfix;
}

void VTKOutCL::CheckFile( const std::ofstream& os) const
/** Checks if a file is open*/
{
    if (!os)
        throw DROPSErrCL( "VTKOutCL: error while opening file!");
}

void VTKOutCL::NewFile(double time, __UNUSED__ bool writeDistribution)
/** Each process opens a new file and writes header into it*/
{
    std::string filename(filename_);
#ifdef _PAR
   ProcCL::AppendProcNum(filename);
   filename+="_";
#endif
    AppendTimecode(filename);
    filename+= ".vtu";
    file_.open((dirname_+filename).c_str());
    if ( !file_){
        CreateDirectory( dirname_);
        file_.open((dirname_+filename).c_str());
    }
    CheckFile( file_);
    PutHeader();
    wrotePointDataLine_= false;
// The file that links the data from all the separate (but nevertheless valid) XML VTK files is exclusively generated by the master-processor
#ifdef _PAR
    IF_MASTER {
        std::string masterfilename( filename_);
        AppendTimecode( masterfilename);
        masterfilename += ".pvtu";
        std::ofstream masterfile( (dirname_+masterfilename).c_str());
        masterfile<<"<?xml version=\"1.0\"?>\n<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n"
                  <<"<PUnstructuredGrid GhostLevel=\"0\">\n"
                  <<"\t<PPoints>\n"
                  <<"\t\t<PDataArray type=\"Float32\" NumberOfComponents=\"3\" "<<(binary_? "format=\"binary\"":"format=\"ascii\"")<<"/>\n"
                  <<"\t</PPoints>";
        WriteVarNames( masterfile, true);
        const VectorBaseCL<float> x;
        for( VTKvarMapT::iterator it=vars_.begin(); it!=vars_.end(); it++)
        {
            WriteValues(x, it->first, it->second->GetDim(), &masterfile);
        }
        masterfile<<"\n\t</PPointData>"
                  <<"\n\t<PCells>\n"
                  <<"\t\t<PDataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\"/>\n"
                  <<"\t\t<PDataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\"/>\n"
                  <<"\t\t<PDataArray type=\"UInt8\" Name=\"types\" format=\"ascii\"/>\n"
                  <<"\t</PCells>\n";
        if (writeDistribution)
            masterfile << "\t<PCellData Scalars=\"processor\">\n"
                       << "\t\t<PDataArray type=\"Int32\" Name=\"processor\" format=\"ascii\"/>\n"
                       << "\t</PCellData>\n";

        for( int p = 0; p < ProcCL::Size(); p++)
        {
            std::string parfilename;
            std::stringstream helper;
            helper << filename_ << "." << std::setfill('0') << std::setw( int( log10( (float)ProcCL::Size()))+1) << p << "_";

            parfilename=helper.str();
            AppendTimecode( parfilename);
            parfilename+= ".vtu";
            masterfile << "<Piece Source=\"" << parfilename << "\"/>\n";
        }
        masterfile << "</PUnstructuredGrid>\n"
                   << "</VTKFile>";
        GenerateTimeFile( time, masterfilename);
    }
#else
        GenerateTimeFile( time, filename);
#endif
}

void VTKOutCL::GenerateTimeFile( double time, const std::string & name) const
{
    std::string timefilename(filename_);
    timefilename+=".pvd";
    timefilename=dirname_+timefilename;
    if(timestep_==0)
    {
        std::ofstream timefile(timefilename.c_str());
        timefile << "<?xml version=\"1.0\"?>\n"
                    "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">\n"
                    "<Collection>\n"
                    "\t<DataSet timestep=\""<< time <<"\" group=\"\" part=\"0\" file=\"" << name <<"\"/>\n"
                    "</Collection>\n"
                    "</VTKFile>";
        timefile.close();
    }
    else
    {
        std::ofstream timefile;
        timefile.open(timefilename.c_str(),std::ios_base::in);
        if(timefile.is_open())
        {
            timefile.seekp(-24,std::ios_base::end);
            timefile << "\t<DataSet timestep=\""<< time <<"\" group=\"\" part=\"0\" file=\"" << name <<"\"/>\n"
                        "</Collection>\n"
                        "</VTKFile>";
            timefile.close();
        }
        else std::cerr << "Could not open VTK Timefile!" << std::endl;
    }
}

void VTKOutCL::PutHeader()
/** Writes the header into the VTK file*/
{
    file_ << "<?xml version=\"1.0\"?>\n"     // this is just the XML declaration, it's unnecessary for the actual VTK file
             "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n"
             "<UnstructuredGrid>\n";
}

void VTKOutCL::PutFooter()
/** Closes the file XML conform*/
{
    file_ <<"\n\t</PointData>"
            "\n</Piece>"
            "\n</UnstructuredGrid>"
            "\n</VTKFile>";
}

void VTKOutCL::GatherCoord()
/** Iterates over all vertices and edges and assigns a consecutive number and their coordinates to them */
{
    // Get number of vertices and edges in last triangulation level
    Uint numVertices=0, numEdges=0;

    for (MultiGridCL::const_TriangVertexIteratorCL it= mg_.GetTriangVertexBegin(lvl_); it!=mg_.GetTriangVertexEnd(lvl_); ++it)
        numVertices++;
    for (MultiGridCL::const_TriangEdgeIteratorCL it= mg_.GetTriangEdgeBegin(lvl_); it!=mg_.GetTriangEdgeEnd(lvl_); ++it)
        numEdges++;

    numPoints_= numLocPoints_= numVertices + numEdges;
    coords_.resize(3*numLocPoints_);

    ///\todo instead of v/eAddrMap_, one could use a P2 index instead (cf. EnsightOutCL)
    Uint counter=0;
    for (MultiGridCL::const_TriangVertexIteratorCL it= mg_.GetTriangVertexBegin(lvl_); it!=mg_.GetTriangVertexEnd(lvl_); ++it){
        // store a consecutive number for the vertex
        vAddrMap_[&*it]= counter;

        // Put coordinate of the vertex into the field of coordinates
        for (int i=0; i<3; ++i)
            coords_[3*counter+i]= (float)it->GetCoord()[i];
        ++counter;
    }

    for (MultiGridCL::const_TriangEdgeIteratorCL it= mg_.GetTriangEdgeBegin(lvl_); it!=mg_.GetTriangEdgeEnd(lvl_); ++it){
        // store a consecutive number for the edge
        eAddrMap_[&*it]= counter;

        // Put coordinate of the barycenter of the edge into the field of coordinates
        const Point3DCL baryCenter= GetBaryCenter(*it);
        for (int i=0; i<3; ++i)
            coords_[3*counter+i]= (float)baryCenter[i];
        ++counter;
    }
}

void Write6Bits( std::ostream& os, char array[8], unsigned int bitposition)
/** This is a helper-function for the WriteBase64 routine. It picks out the right bit sequence and translates it into one ASCII character */
{
    unsigned char bitmask1,bitmask2,bitsequence1,bitsequence2;                                  // These are all helping variables for the reading process of the bitsequences.
    char base64string[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    switch(bitposition%8)                                                                       // Due to the fact that each byte has 8 bit and we read 6 bits every step, there are just 4 possibilities for positions in one byte. This switch covers those possibilites.
    {                                                                                           // The modulo operator will give the position and the integer division will give back the byte one is in.
    case 0:
        bitmask1=0xFC;                                                                          // 0xFC=11111100 as Bitsequence
        bitsequence1=(array[bitposition/8]&bitmask1);                                           // The integerdivision will result in the correct byte to check (see above). Checking whether the left 6 Bits are set.
        bitsequence1>>=2;                                                                       // Rightshift of the Bitsequence, because the sequence must start at the 0-bit.
        os<<base64string[bitsequence1];                                                       // Interpreting the sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        break;
    case 2:
        bitmask1=0x3F;                                                                          // 0x3F=00111111 as Bitsequence
        bitsequence1=(array[bitposition/8]&bitmask1);                                           // Checking whether the right 6 Bits are set. No shift nessecary this time.
        os<<base64string[bitsequence1];                                                         // Interpreting the sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        break;
    case 4:
        bitmask1=0x0F;                                                                          // 0x0F=00001111 as Bitsequence
        bitmask2=0xC0;                                                                          // 0xC0=11000000 as Bitsequence
        bitsequence1=(array[bitposition/8]&bitmask1);                                           // Checking whether the right 4 Bits are set.
        bitsequence2=(array[bitposition/8+1]&bitmask2);                                         // Checking whether the left 2 Bits are set.
        bitsequence1<<=2;                                                                       // Shifting the first Bitsequence two the left by two bits.
        bitsequence2>>=6;                                                                       // Shifting the second Bitsequence two the right by six bits.
        bitsequence1=(bitsequence1|bitsequence2);                                               // Combining the two Bitsequences
        os<<base64string[bitsequence1];                                                         // Interpreting the final sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        break;
    case 6:
        bitmask1=0x03;                                                                          // 0x03=00000011 as Bitsequence
        bitmask2=0xF0;                                                                          // 0xF0=11110000 as Bitsequence
        bitsequence1=(array[bitposition/8]&bitmask1);                                           // Checking whether the right 2 Bits are set.
        bitsequence2=(array[bitposition/8+1]&bitmask2);                                         // Checking whether the left 4 Bits are set.
        bitsequence1<<=4;                                                                       // Shifting the first Bitsequence two the left by four bits.
        bitsequence2>>=4;                                                                       // Shifting the second Bitsequence two the right by four bits.
        bitsequence1=(bitsequence1|bitsequence2);                                               // Combining the two Bitsequences
        os<<base64string[bitsequence1];                                                         // Interpreting the final sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        break;
    }
}

template <class T>
void WriteBase64( const VectorBaseCL<T>& x, std::ostream& os)
/** Writes out a base64 encoded data-stream */
{
    unsigned int end=x.size()*sizeof(T);                                                            // This is the size-indicator which the reading routine needs in order to know how much data will come afterwards.
    if (end<1) return;                                                                              // There is nothing to do if the vector is empty.
    char base64string[65]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";       // This string characterizes which number corresponds to which base64 character.
    union Tvsc                                                                                      // Union is used to get the bit sequence of the Templatevariable into a char-variable. Another way to do this would be memcpy.
    {
        T f[2];
        char c[8];
    };
    Tvsc Templatevschar;
    memcpy(&Templatevschar.f[0],&end,4);                                                            // This is the alternative way described above. The size indicator is put into the first part of the union.
    Templatevschar.f[1]=x[0];                                                                       // The starting point for the loop had to be set manually, because of the length indicator which must be the first entry to come.
    unsigned int bitposition=0;                                                                     // Bitposition is always used as the current starting position for the next 6 bits to read.
    unsigned char bitmask1,bitsequence1;                                                            // These are all helping variables for the reading process of the bitsequences.
    unsigned int a=1;
    while(a<x.size())                                                                               // As long as one did not reach the last entry of the vector...
    {                                                                                               // {
        if(bitposition>=32)                                                                         // This means the starting index for the next reading process reached the second entry of f[2]
        {                                                                                           // if this is the case then...
            Templatevschar.f[0]=Templatevschar.f[1];                                                // the first entry is replace with the second one
            Templatevschar.f[1]=x[a];                                                               // the second entry is replaced with the next vector-entry of x
            ++a;                                                                                    // the entry counter for x is increased
            bitposition-=32;                                                                        // and the bitposition is decreased by 32, which is exactly the amount of bits one entry of f[2] has. So after all one is still at the same point in the bitstream!
        }
        Write6Bits( os, Templatevschar.c, bitposition);
        bitposition+=6;                                                                             // Increasing the bitposition by 6, because this is the amount of bits one reads every step.
    }                                                                                               // }
    // Padding. When the last vector entry is reached and a is set to "end" the while loop above will end after one final run. The bitposition variable will then be somewhere between 6 and 12. The vectorentry x[end-1] will be stored in f[1] and therefor
    // we will have to continue the process until we reach bitposition 64. This is what we do here.
    while(bitposition<58)
    {
        Write6Bits( os, Templatevschar.c, bitposition);
        bitposition+=6;
    }
    // The real padding-process starts here actually. We now have to check, whether the process ends with bit 64 precisely or whether we have to append zerobytes to make the bytesequence, which obviously has to be divisible by 8, also divisible by 6.
    if(bitposition==58)                                                                             // Either one is at bit 58, which means that one has exactly one character left to write and does not need to append zero-bytes.
    {
        bitmask1=0x3F;                                                                              // Then one takes the last six bits (remember 0x3F=00111111 as bitsequence)
        bitsequence1=Templatevschar.c[7]&bitmask1;                                                  // Checking whether the right 6 Bits are set.
        os <<base64string[bitsequence1];                                                            // Interpreting the sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
    }
    if(bitposition==60)                                                                             // Or one is at bit 60, so that one will need 1 extra byte (because 60+6=66 is not divisible by 8, contrary to 66+6+6=72=64+8)
    {
        bitmask1=0x0F;                                                                              // Then one takes the last four bits (remember 0x0F=00001111 as bitsequence)
        bitsequence1=Templatevschar.c[7]&bitmask1;                                                  // Checking whether the right 4 Bits are set.
        bitsequence1<<=2;                                                                           // Shifting the bitsequence 2 bits to the left, which means inserting two zeros from the right.
        os <<base64string[bitsequence1];                                                            // Interpreting the sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        os <<"=";                                                                                   // Appending one equal sign to the file, because every extra byte will lead to one equal sign. The zeros in the extra byte will not be transformed into "A"s to make it clear that they don't belong to the string in the first place!
    }
    if(bitposition==62)                                                                             // Or one is at bit 62, so that one will need 2 extra byte (because 62+6=68 is not divisible by 8, contrary to 62+6+6+6=80=64+8+8)
    {
        bitmask1=0x03;                                                                              // Then one takes the last two bits (remember 0x03=00000011 as bitsequence)
        bitsequence1=Templatevschar.c[7]&bitmask1;                                                  // Checking whether the right 2 Bits are set.
        bitsequence1<<=4;                                                                           // Shifting the bitsequence 2 bits to the left, which means inserting two zeros from the right.
        os <<base64string[bitsequence1];                                                            // Interpreting the sequence as Number between 0 and 63 and writing the corresponding base64 character into the file_
        os <<"==";                                                                                  // Appending two equal signs to the file_, due to the two needed extra bytes.
    }
}

void VTKOutCL::WriteCoords()
/** Each process writes out its coordinates. */
{
    file_<< "<Piece NumberOfPoints=\""<<numPoints_<<"\" NumberOfCells=\""<<numTetras_<<"\">"
            "\n\t<Points>"
            "\n\t\t<DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"" << ( binary_ ? "binary\">\n\t\t" : "ascii\">\n\t\t");

    if (binary_)
        WriteBase64(coords_, file_);
    else
        for (Uint i=0; i<numPoints_; ++i)
            file_<< coords_[3*i+0] << ' ' << coords_[3*i+1] << ' ' << coords_[3*i+2]<< ' ';

    file_<< "\n\t\t</DataArray> \n"
            "\t</Points>\n";
}

void VTKOutCL::GatherTetra()
/** Gathers tetrahedra in an array*/
{
    numTetras_=std::distance(mg_.GetTriangTetraBegin(lvl_),mg_.GetTriangTetraEnd(lvl_));  // calculates the number of tetrahedra
    tetras_.resize((onlyP1_?4:10)*numTetras_);      // (four vertices) * Number of Tetrahedra respectively (four vertices + six edges) * Number of Tetrahedra

    // Gathers connectivities
    Uint counter=0;
    for (MultiGridCL::const_TriangTetraIteratorCL it= mg_.GetTriangTetraBegin(lvl_); it!=mg_.GetTriangTetraEnd(lvl_); ++it){ //loop over all tetrahedra
        for (int vert= 0; vert<4; ++vert)
            tetras_[counter++] = vAddrMap_[it->GetVertex(vert)];
        if(!onlyP1_)
        {
            for (int eddy=0; eddy<6; ++eddy)
                tetras_[counter++] = eAddrMap_[it->GetEdge(eddy)];
            std::swap(tetras_[counter-4],tetras_[counter-5]);    // Permutation needed to make DROPS and VTK compatible (different numeration)
        }
    }
    Assert(counter==(onlyP1_? 4:10)*numTetras_, DROPSErrCL("VTKOutCL::GatherTetra: Mismatching number of tetrahedra"), ~0);
}

void VTKOutCL::WriteTetra( bool writeDistribution)
/** Writes the tetrahedra into the VTK file*/
{
    file_   << "\t<Cells>\n"
               "\t\t<DataArray type=\"Int32\" Name=\"connectivity\" format=\"";
// Binary output for the connectivity data seems useless (using >5 byte per integer), because it only blows up the amount of needed storage space, but it's implemented anyway,
// for the sake of completeness.
//  if(binary_)
//  {
//      file_<<"binary\">"/*\n\t\t*/;
//      // Write out connectivities
//      WriteBase64(tetras_, file_);
//  }
//  else
//  {
        file_   <<"ascii\">\n\t\t";
        // Write out connectivities
        if(onlyP1_)
            for (Uint i=0; i<numTetras_; ++i)
            {
                file_ << tetras_[4*i+0] << ' '<< tetras_[4*i+1] << ' '<< tetras_[4*i+2] << ' '<< tetras_[4*i+3] << " ";
            }
        else
            for (Uint i=0; i<numTetras_; ++i)
            {
                file_ << tetras_[10*i+0] << ' '<< tetras_[10*i+1] << ' '<< tetras_[10*i+2] << ' '<< tetras_[10*i+3] << ' '
                      << tetras_[10*i+4] << ' '<< tetras_[10*i+5] << ' '<< tetras_[10*i+6] << ' '<< tetras_[10*i+7] << ' '
                      << tetras_[10*i+8] << ' '<< tetras_[10*i+9] << " ";
            }
//  }
    file_ << "\n\t\t</DataArray>\n"
             "\t\t<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n\t\t";
    if(onlyP1_)
        for(Uint i=1; i<=numTetras_; ++i) file_ << i*4<<" ";
    else
        for(Uint i=1; i<=numTetras_; ++i) file_ << i*10<<" ";
    file_ << "\n\t\t</DataArray>"
             "\n\t\t<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n\t\t";
    const char* tetraType= (onlyP1_? "10 ":"24 ");
    for(Uint i=1; i<=numTetras_; ++i)
        file_ << tetraType;
    file_ << "\n\t\t</DataArray>"
             "\n\t</Cells>";

    if ( writeDistribution)
        WriteDistribution();
}

void VTKOutCL::WriteDistribution()
/** Writes the distribution-data into the file (as CellData)*/
{
#ifdef _PAR
    file_ << '\n'
          << "\t<CellData>\n"
          << "\t\t<DataArray type=\"Int32\" Name=\"processor\" format=\"ascii\">\n"
          << "\t\t";
    int c=ProcCL::MyRank();
    for( Uint i=0; i < numTetras_; ++i)
            file_<< c << " ";
    file_   << "\n\t\t</DataArray>\n"
            << "\t</CellData>\n";
#endif
}

void VTKOutCL::WriteVarNames(std::ofstream& file, bool masterfile)
{
    std::vector<std::string> scalarvalued;
    std::vector<std::string> vectorvalued;

    for (VTKvarMapT::iterator it= vars_.begin(); it != vars_.end(); ++it)
    {
        if (it->second->GetDim()==1) scalarvalued.push_back(it->first);
        if (it->second->GetDim()==3) vectorvalued.push_back(it->first);
    }
    file << "\n\t<" << (masterfile? "P":"") << "PointData ";

    if(!scalarvalued.empty())
    {
        file << "Scalars=\"" << scalarvalued[0];
        for (size_t i= 1; i < scalarvalued.size(); ++i)
            file << "," << scalarvalued[i];
        file << "\"";
    }
    if (!vectorvalued.empty())
    {
        if (!scalarvalued.empty())
            file << " ";
        file << "Vectors=\"" << vectorvalued[0];
        for (size_t i= 1; i<vectorvalued.size();++i)
            file << "," << vectorvalued[i];
        file << "\"";
    }

    file << ">";
}

void VTKOutCL::WriteValues( const VectorBaseCL<float>& allData, const std::string& name, int numData, std::ofstream* filePtr)
/** Writes out the calculated numerical data*/
{
    std::ofstream& file= filePtr ? *filePtr : file_;

    file << "\n\t\t<" << ( !filePtr ? "" : "P") << "DataArray type=\"Float32\" Name=\"" << name << "\""
            " NumberOfComponents=\"" << numData << "\" format=\""
         << ( binary_ ? "binary\"" : "ascii\"") << ( !filePtr ? "" : "/") << ">";
    if( !filePtr) // omit for master file
    {
        file << "\n\t\t";
        if ( binary_)
            WriteBase64(allData, file_);
        else {
            for ( Uint i=0; i<allData.size(); ++i)
                file << allData[i] << ' ';
        }
        file << "\n\t\t</DataArray>";
    }
}

void VTKOutCL::PutGeom(double time, bool writeDistribution)
/** At first the geometry is put into the VTK file. Therefore this procedure
    opens the file and writes description into the file.
    \param writeDistribution Flag indicator whether distribution-data should be written in the file (as CellData)
*/
{
    NewFile(time,writeDistribution);
    Clear();
    GatherCoord();
    GatherTetra();
    WriteCoords();
    WriteTetra( writeDistribution);
    WriteVarNames(file_,/*masterfile=*/0);
}

void VTKOutCL::Clear()
{
    if (numPoints_>0){

        vAddrMap_.clear();
        eAddrMap_.clear();
        coords_.resize(0);
        tetras_.resize(0);
    }
}

} // end of namespace DROPS
