//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/ExtractVOI/
//Extract a volume of interest (subvolume).
//提取感兴趣的volume
//VOI, Volume Of Interest，感兴趣的体积（相似概念POI）
//@2D@Image
//@Clip@Visualization
#include <vtkExtractVOI.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    //
    //创建一个Image
    //
    vtkNew<vtkImageMandelbrotSource> source; //Mandelbrot图像
    source->Update();
    vtkImageData* image = source->GetOutput();

    int* inputDims = image->GetDimensions(); //此图像的维度（x;y;z的范围）
    std::cout << "Dims: "
        << " x: " << inputDims[0]
		<< " y: " << inputDims[1]
        << " z: " << inputDims[2] //此为二维图像，所以z=1
		<< std::endl;
    std::cout << "Number of points: " << image->GetNumberOfPoints() << std::endl;   //此网格的点数：X范围*Y范围*Z范围
    std::cout << "Number of cells: " << image->GetNumberOfCells() << std::endl;     //点网格的单元数（像元、体元）的个数

    //
    // 感兴趣的体积
    //
    vtkNew<vtkExtractVOI> extractVOI;
    extractVOI->SetInputConnection(source->GetOutputPort()); //设置需要裁剪的数据
    extractVOI->SetVOI(
        inputDims[0] / 4., 
        3. * inputDims[0] / 4.,
        inputDims[1] / 4., 3. * inputDims[1] / 4., 0, 0
    );
    extractVOI->Update();

    vtkImageData* extracted = extractVOI->GetOutput();

    //
    // 裁剪结果
    //
    int* extractedDims = extracted->GetDimensions();
    std::cout << "Dims: "
        << " x: " << extractedDims[0]
		<< " y: " << extractedDims[1]
        << " z: " << extractedDims[2]
		<< std::endl;
    std::cout << "Number of points: " << extracted->GetNumberOfPoints() << std::endl;
    std::cout << "Number of cells: " << extracted->GetNumberOfCells() << std::endl;

    //
    // 可视化
    //
    //将vtkImageMandelbrotSource转换为能够被 Image处理管线 处理的类型
    vtkNew<vtkImageCast> inputCastFilter;
    inputCastFilter->SetInputConnection(source->GetOutputPort());
    inputCastFilter->SetOutputScalarTypeToUnsignedChar();
    inputCastFilter->Update();

    vtkNew<vtkImageCast> extractedCastFilter;
    extractedCastFilter->SetInputData(extracted);
    extractedCastFilter->SetOutputScalarTypeToUnsignedChar();
    extractedCastFilter->Update();

    // Create actors
    vtkNew<vtkImageActor> inputActor;
    inputActor->GetMapper()->SetInputConnection(inputCastFilter->GetOutputPort());

    vtkNew<vtkImageActor> extractedActor;
    extractedActor->GetMapper()->SetInputConnection(
        extractedCastFilter->GetOutputPort());

    // There will be one render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(600, 300);

    // And one interactor
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    //定义视口范围
    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    //创建两个舞台（渲染器、视口）
    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer);
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(colors->GetColor3d("Burlywood").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer);
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(colors->GetColor3d("DarkTurquoise").GetData());

    leftRenderer->AddActor(inputActor);
    rightRenderer->AddActor(extractedActor);

    leftRenderer->ResetCamera();

    rightRenderer->ResetCamera();
    renderWindow->SetWindowName("ExtractVOI");
    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}