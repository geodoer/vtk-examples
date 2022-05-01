//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/ExtractVOI/
//Extract a volume of interest (subvolume).
//��ȡ����Ȥ��volume
//VOI, Volume Of Interest������Ȥ����������Ƹ���POI��
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
    //����һ��Image
    //
    vtkNew<vtkImageMandelbrotSource> source; //Mandelbrotͼ��
    source->Update();
    vtkImageData* image = source->GetOutput();

    int* inputDims = image->GetDimensions(); //��ͼ���ά�ȣ�x;y;z�ķ�Χ��
    std::cout << "Dims: "
        << " x: " << inputDims[0]
		<< " y: " << inputDims[1]
        << " z: " << inputDims[2] //��Ϊ��άͼ������z=1
		<< std::endl;
    std::cout << "Number of points: " << image->GetNumberOfPoints() << std::endl;   //������ĵ�����X��Χ*Y��Χ*Z��Χ
    std::cout << "Number of cells: " << image->GetNumberOfCells() << std::endl;     //������ĵ�Ԫ������Ԫ����Ԫ���ĸ���

    //
    // ����Ȥ�����
    //
    vtkNew<vtkExtractVOI> extractVOI;
    extractVOI->SetInputConnection(source->GetOutputPort()); //������Ҫ�ü�������
    extractVOI->SetVOI(
        inputDims[0] / 4., 
        3. * inputDims[0] / 4.,
        inputDims[1] / 4., 3. * inputDims[1] / 4., 0, 0
    );
    extractVOI->Update();

    vtkImageData* extracted = extractVOI->GetOutput();

    //
    // �ü����
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
    // ���ӻ�
    //
    //��vtkImageMandelbrotSourceת��Ϊ�ܹ��� Image������� ���������
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

    //�����ӿڷ�Χ
    // Define viewport ranges
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    //����������̨����Ⱦ�����ӿڣ�
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