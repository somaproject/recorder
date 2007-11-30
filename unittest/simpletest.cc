#include <iostream>
#include <H5Cpp.h>
#include <hdf5.h>

using namespace H5;

void stringtest()
{
  std::cout << "Simple create string test" << std::endl;

  H5::H5File h5file("test.h5", H5F_ACC_TRUNC); 

  const int SPACE1_RANK = 1;
  const hsize_t SPACE1_DIM1 = 4;
  const char *wdata[SPACE1_DIM1]= {
    "Four score and seven years ago our forefathers brought forth on this continent a new nation,",
    "conceived in liberty and dedicated to the proposition that all men are created equal.",
    "Now we are engaged in a great civil war,",
    "testing whether that nation or any nation so conceived and so dedicated can long endure."
  };   // Information to write


  // Create dataspace for datasets.
  hsize_t	dims1[] = {SPACE1_DIM1};
  DataSpace sid1(SPACE1_RANK, dims1);
  
  // Create a datatype to refer to.
  StrType tid1(0, H5T_VARIABLE);
  
  // Create and write a dataset.
  DataSet dataset(h5file.createDataSet("Dataset1", tid1, sid1));
  dataset.write(wdata, tid1);
  
  // Create H5S_SCALAR data space.
  DataSpace scalar_space;
  
  // Create and write another dataset.
//   DataSet dataset2(file1->createDataSet("Dataset2", tid1, scalar_space));
//   char *wdata2 = (char*)HDcalloc(65534, sizeof(char));
//   HDmemset(wdata2, 'A', 65533);
//   dataset2.write(&wdata2, tid1);
  
  
}

void attrtest()
{
  std::cout << "Simple create attribute test" << std::endl;
  // create the file
  H5::H5File h5file("test.h5", H5F_ACC_TRUNC); 

  // create the group
  H5::Group newGroup =  h5file.createGroup("testGroup");

  H5::Group newGroup2 =  h5file.openGroup("testGroup");
  
  H5::DataSpace attrspace; 
  
  // add attributes
  H5::Attribute createtime = newGroup.createAttribute("Units", 
						      H5::PredType::NATIVE_LLONG,
						      attrspace); 
  int x = 1000; 
  createtime.write(H5::PredType::NATIVE_LLONG, &x); 


}
int main()
{
  stringtest(); 

}
