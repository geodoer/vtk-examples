//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/GetCellCenter/
//Get the coordinates of the center of a cell.
#include <vtkNew.h>
#include <vtkCell.h>
#include <vtkImageData.h>

namespace {
    //��õ�Ԫ�����ĵ�
    void GetCellCenter(vtkImageData* imageData, const unsigned int cellId,
        double center[3]);
}


int main(int, char* [])
{
    // Create an image data
    vtkNew<vtkImageData> imageData;

    // Specify the size of the image data
    imageData->SetDimensions(3, 3, 2); // This will cause 18 points and 4 cells
    imageData->SetSpacing(1.0, 1.0, 1.0);
    imageData->SetOrigin(0.0, 0.0, 0.0);

    std::cout << "Number of points: " << imageData->GetNumberOfPoints() << std::endl;
    std::cout << "Number of cells: " << imageData->GetNumberOfCells() << std::endl;

    double center[3] = { 0, 0, 0 };
    for (vtkIdType cellId = 0; cellId < imageData->GetNumberOfCells(); ++cellId)
    {
        GetCellCenter(imageData, cellId, center);

        std::cout << "Cell " << cellId
    		<< " center: " << center[0] << " " << center[1] << " " << center[2] << std::endl;
    }

    return EXIT_SUCCESS;
}

namespace {

    void GetCellCenter(vtkImageData* imageData, const unsigned int cellId, double center[3])
    {
        double pcoords[3] = { 0, 0, 0 };

        int size = imageData->GetMaxCellSize(); //��Ԫ���ܣ����������Voxel���������8����size=8
        double* weights = new double[size]; //��Ԫ���У�ÿ�����Ȩ����

        //���ݵ�ԪID��ȡ��Ԫ
        //cell�����ͣ�vtkVoxel
        vtkCell* cell = imageData->GetCell(cellId); 
        //�����������ģ�i,j,k���꣬������ģ������ϵ��
        //@pcoords  ����cell�ĵ�Ԫ������
        //@subId    �������ڵ�subId
        int subId = cell->GetParametricCenter(pcoords);
        //��subId��pcoords������center��weights
        cell->EvaluateLocation(subId, pcoords, center, weights);

        std::cout << "\tWeights";
        for(auto i(0); i<size; ++i)
        {
            std::cout << weights[i] << "\t";
        }
        std::cout << std::endl;
    }
} // namespace
