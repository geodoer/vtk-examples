//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/GetCellCenter/
//Get the coordinates of the center of a cell.
#include <vtkNew.h>
#include <vtkCell.h>
#include <vtkImageData.h>

namespace {
    //获得单元的中心点
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

        int size = imageData->GetMaxCellSize(); //单元格总，点的总数。Voxel点的数量是8，故size=8
        double* weights = new double[size]; //单元格中，每个点的权重数

        //根据单元ID获取单元
        //cell的类型：vtkVoxel
        vtkCell* cell = imageData->GetCell(cellId); 
        //计算网格中心（i,j,k坐标，并不是模型坐标系）
        //@pcoords  返回cell的单元格中心
        //@subId    中心所在的subId
        int subId = cell->GetParametricCenter(pcoords);
        //从subId、pcoords，计算center、weights
        cell->EvaluateLocation(subId, pcoords, center, weights);

        std::cout << "\tWeights";
        for(auto i(0); i<size; ++i)
        {
            std::cout << weights[i] << "\t";
        }
        std::cout << std::endl;
    }
} // namespace
