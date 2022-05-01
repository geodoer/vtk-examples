//https://kitware.github.io/vtk-examples/site/Cxx/ImageData/ClipVolume/
// This program draws a checkerboard and clips it with two planes.

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkClipVolume.h>
#include <vtkColor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExecutive.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderStepsPass.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

namespace
{
	//����һ��ķ�����
	constexpr auto IMAGESIZE{64}; // number of checkerboard squares on a side
	constexpr auto CUBESIZE{20.0}; // physical linear dimension of entire system

	// Color for the checkerboard image
	constexpr auto DIM{0.5}; // amount to dim the dark squares by

	// Offsets for clipping planes with normals in the X and Y directions
	constexpr auto XOFFSET{8};
	constexpr auto YOFFSET{8};

	//����ImageData����һ���򵥵�����
	//@vtkImageData@����@����@��ɫ���ұ�
	vtkSmartPointer<vtkImageData> makeImage(int n, vtkColor3d fillColor, vtkColor3d checkerColor);
}

int main(int, char*[])
{
	/*ʹ��PolygonOffset����
	 *�����Ƿ���λ�غ�����(���磬������ϵ�һ����)���Ա���z-buffer�ֱ���(�Ӷ�������Ⱦ����)
	 *VTK�����ֲ���
	 *1. PolygonOffsetʹ��ͼ��ϵͳ�������ƶ�����Ρ��ߺ͵�
	 *2. ShiftZBuffer��һ�������ķ�������������ӳ��z-buffer�������ֶ��㡢�ߺͶ���Σ����������ǲ����ɽ��ܵĽ��
	 */
	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

	//
	//������ɫ
	//
	vtkNew<vtkNamedColors> colors;
	vtkColor3d backgroundColor = colors->GetColor3d("Wheat");
	vtkColor3d checkerColor = colors->GetColor3d("Tomato");
	vtkColor3d fillColor = colors->GetColor3d("Banana");

	//������������
	auto image = makeImage(IMAGESIZE, fillColor, checkerColor);

	//
	//�������ü�ƽ��
	//
	vtkNew<vtkDoubleArray> normals;
	vtkNew<vtkPoints> clipPts;
	normals->SetNumberOfComponents(3);
	double xnorm[3] = {-1., 0., 0.};
	double ynorm[3] = {0., -1., 0.};
	double xpt[3] = {XOFFSET, 0., 0.};
	double ypt[3] = {0., YOFFSET, 0.};
	normals->InsertNextTuple(xnorm);
	normals->InsertNextTuple(ynorm);
	clipPts->InsertNextPoint(xpt);
	clipPts->InsertNextPoint(ypt);

	//@vtkPlanes@ʹ��
	vtkNew<vtkPlanes> clipPlanes;
	clipPlanes->SetNormals(normals);
	clipPlanes->SetPoints(clipPts);

	//
	//�ɶ���ü�ƽ�湹��ü����
	//
	//@vtkClipVolume@ʹ��
	vtkNew<vtkClipVolume> clipper;
	clipper->SetClipFunction(clipPlanes);
	clipper->SetInputData(image);

	{
		//
		// ����鿴�ü����
		//
		vtkNew<vtkXMLUnstructuredGridWriter> writer;
		writer->SetInputConnection(clipper->GetOutputPort());
		writer->SetFileName("[ClipVolume] checkerboard-clip.vtu");
		writer->Write();
	}

	//
	//��Ⱦ
	//
	vtkNew<vtkRenderer> renderer;
	renderer->SetBackground(backgroundColor.GetData());

	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(640, 480);

	vtkNew<vtkRenderWindowInteractor> interactor;
	vtkNew<vtkInteractorStyleSwitch> style;
	interactor->SetInteractorStyle(style);
	interactor->SetRenderWindow(renderWindow);

	vtkNew<vtkDataSetMapper> imageMapper;
	vtkNew<vtkActor> imageActor;
	imageActor->SetMapper(imageMapper);
	renderer->AddViewProp(imageActor);
	imageMapper->SetInputConnection(clipper->GetOutputPort());

	renderer->ResetCamera();
	renderer->GetActiveCamera()->Azimuth(120);
	renderer->GetActiveCamera()->Elevation(30);
	renderer->ResetCameraClippingRange();
	renderWindow->SetWindowName("ClipVolume");
	renderWindow->Render();

	interactor->Start();
	return EXIT_SUCCESS;
}

namespace
{
	vtkSmartPointer<vtkImageData> makeImage(int n, vtkColor3d fillColor,
	                                        vtkColor3d checkerColor)
	{
		vtkNew<vtkImageData> image0;

		//
		// ���������Ƶ�ImageData
		//

		//����ά��
		image0->SetDimensions(n, n, n);
		//��������Ԫ��ͨ������ֵ����
		image0->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		//��������Ԫ֮��ľ���
		image0->SetSpacing(CUBESIZE / n, CUBESIZE / n, CUBESIZE / n);
		//�����̸�ֵ
		int checkerSize = n / 8;
		for (int z = 0; z < n; z++)
		{
			for (int y = 0; y < n; y++)
			{
				for (int x = 0; x < n; x++)
				{
					//���xyz��Ԫ
					unsigned char* ptr = (unsigned char*)image0->GetScalarPointer(x, y, z);
					//��ֵ
					*ptr = (x / checkerSize + y / checkerSize + z / checkerSize) % 2; // checkerboard
				}
			}
		}

		//
		// ��ɫ���ұ�
		//
		//@vtkLookupTable@ʹ��
		vtkNew<vtkLookupTable> lut;
		lut->SetNumberOfTableValues(2);		//ָ��������2��
		lut->SetTableRange(0, 1);	//ֵ��Χ��[0, 1]
		lut->SetTableValue(0,				//��ֵ0 ӳ��� fillColor
			fillColor.GetRed(), fillColor.GetGreen(), fillColor.GetBlue(), 1.0
		);
		lut->SetTableValue(1,				//��ֵ1 ӳ��� checkerColor
			checkerColor.GetRed(), checkerColor.GetGreen(), checkerColor.GetBlue(), 1.0
		);

		//
		// ΪvtkImageData������ɫ���ұ�
		//
		auto map = vtkSmartPointer<vtkImageMapToColors>::New();
		map->SetLookupTable(lut);
		map->SetOutputFormatToRGBA();
		map->SetInputData(image0);
		map->GetExecutive()->Update();

		//
		// ����鿴
		//
		vtkNew<vtkXMLImageDataWriter> writer;
		writer->SetFileName("[ClipVolume] checkerboard.vti");
		writer->SetInputData(map->GetOutput());
		writer->Write();

		return map->GetOutput();
	}
}