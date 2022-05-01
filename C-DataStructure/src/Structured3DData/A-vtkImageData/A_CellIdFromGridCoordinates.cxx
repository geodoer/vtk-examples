//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/CellIdFromGridCoordinates
//�������������ȡ��Ԫ���id
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkStructuredData.h>

int main(int, char*[])
{
	vtkNew<vtkImageData> grid;
	grid->SetOrigin(0, 0, 0);			//����ԭ������꣨ģ������ϵ��

	unsigned int numVoxelsPerDimension = 2;	//ÿ��ά���е����أ�����Ԫ������
	grid->SetSpacing(1, 1, 1);		//���صļ������ȡ��߶ȡ����ȣ�

	//��������ķ�Χ����Ϊi,j,k�ķ�Χ��������ģ������ϵ�ķ�Χ��
	int extent[6];
	extent[0] = 0;
	extent[1] = numVoxelsPerDimension; //i��[0, numVoxelsPerDimension)
	extent[2] = 0;
	extent[3] = numVoxelsPerDimension; //j��[0, numVoxelsPerDimension) 
	extent[4] = 0;
	extent[5] = numVoxelsPerDimension; //z��[0, numVoxelsPerDimension]
	grid->SetExtent(extent);

	//���������
	//�������ͣ�VTK_INT
	//������1��
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

				//���ݷ�Χ��ijk��ȡ��id
				vtkIdType id = vtkStructuredData::ComputeCellIdForExtent(extent, pos);
				std::cout << "Cell " << i << " " << j << " " << k << " has id : " << i << std::endl;
			}
		}
	}

	return EXIT_SUCCESS;
}
