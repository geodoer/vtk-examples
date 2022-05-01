//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/CellIdFromGridCoordinates
//根据网格坐标获取单元格的id
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkStructuredData.h>

int main(int, char*[])
{
	vtkNew<vtkImageData> grid;
	grid->SetOrigin(0, 0, 0);			//网格原点的坐标（模型坐标系）

	unsigned int numVoxelsPerDimension = 2;	//每个维度中的体素（网格单元）数量
	grid->SetSpacing(1, 1, 1);		//体素的间隔（宽度、高度、长度）

	//网格坐标的范围（此为i,j,k的范围，而不是模型坐标系的范围）
	int extent[6];
	extent[0] = 0;
	extent[1] = numVoxelsPerDimension; //i∈[0, numVoxelsPerDimension)
	extent[2] = 0;
	extent[3] = numVoxelsPerDimension; //j∈[0, numVoxelsPerDimension) 
	extent[4] = 0;
	extent[5] = numVoxelsPerDimension; //z∈[0, numVoxelsPerDimension]
	grid->SetExtent(extent);

	//分配标量点
	//数据类型：VTK_INT
	//分量：1个
	grid->AllocateScalars(VTK_INT, 1);

	for (unsigned int i = 0; i < numVoxelsPerDimension; ++i)
	{
		for (unsigned int j = 0; j < numVoxelsPerDimension; ++j)
		{
			for (unsigned int k = 0; k < numVoxelsPerDimension; ++k)
			{
				int pos[3];
				pos[0] = i;
				pos[1] = j;
				pos[2] = k;

				//根据范围、ijk获取点id
				vtkIdType id = vtkStructuredData::ComputeCellIdForExtent(extent, pos);
				std::cout << "Cell " << i << " " << j << " " << k << " has id : " << i << std::endl;
			}
		}
	}

	return EXIT_SUCCESS;
}
