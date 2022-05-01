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
	//棋盘一侧的方个数
	constexpr auto IMAGESIZE{64}; // number of checkerboard squares on a side
	constexpr auto CUBESIZE{20.0}; // physical linear dimension of entire system

	// Color for the checkerboard image
	constexpr auto DIM{0.5}; // amount to dim the dark squares by

	// Offsets for clipping planes with normals in the X and Y directions
	constexpr auto XOFFSET{8};
	constexpr auto YOFFSET{8};

	//创建ImageData，是一个简单的棋盘
	//@vtkImageData@创建@棋盘@颜色查找表
	vtkSmartPointer<vtkImageData> makeImage(int n, vtkColor3d fillColor, vtkColor3d checkerColor);
}

int main(int, char*[])
{
	/*使用PolygonOffset方法
	 *对于是否移位重合拓扑(例如，多边形上的一条线)，以避免z-buffer分辨率(从而导致渲染问题)
	 *VTK有两种策略
	 *1. PolygonOffset使用图形系统调用来移动多边形、线和点
	 *2. ShiftZBuffer是一个遗留的方法，用于重新映射z-buffer，以区分顶点、线和多边形，但并不总是产生可接受的结果
	 */
	vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

	//
	//定义颜色
	//
	vtkNew<vtkNamedColors> colors;
	vtkColor3d backgroundColor = colors->GetColor3d("Wheat");
	vtkColor3d checkerColor = colors->GetColor3d("Tomato");
	vtkColor3d fillColor = colors->GetColor3d("Banana");

	//创建棋盘网格
	auto image = makeImage(IMAGESIZE, fillColor, checkerColor);

	//
	//构造多个裁剪平面
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

	//@vtkPlanes@使用
	vtkNew<vtkPlanes> clipPlanes;
	clipPlanes->SetNormals(normals);
	clipPlanes->SetPoints(clipPts);

	//
	//由多个裁剪平面构造裁剪体积
	//
	//@vtkClipVolume@使用
	vtkNew<vtkClipVolume> clipper;
	clipper->SetClipFunction(clipPlanes);
	clipper->SetInputData(image);

	{
		//
		// 保存查看裁剪结果
		//
		vtkNew<vtkXMLUnstructuredGridWriter> writer;
		writer->SetInputConnection(clipper->GetOutputPort());
		writer->SetFileName("[ClipVolume] checkerboard-clip.vtu");
		writer->Write();
	}

	//
	//渲染
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
		// 创建棋盘似的ImageData
		//

		//设置维度
		image0->SetDimensions(n, n, n);
		//设置网格单元的通道与数值类型
		image0->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		//设置网格单元之间的距离
		image0->SetSpacing(CUBESIZE / n, CUBESIZE / n, CUBESIZE / n);
		//给棋盘赋值
		int checkerSize = n / 8;
		for (int z = 0; z < n; z++)
		{
			for (int y = 0; y < n; y++)
			{
				for (int x = 0; x < n; x++)
				{
					//获得xyz单元
					unsigned char* ptr = (unsigned char*)image0->GetScalarPointer(x, y, z);
					//赋值
					*ptr = (x / checkerSize + y / checkerSize + z / checkerSize) % 2; // checkerboard
				}
			}
		}

		//
		// 颜色查找表
		//
		//@vtkLookupTable@使用
		vtkNew<vtkLookupTable> lut;
		lut->SetNumberOfTableValues(2);		//指定数量，2个
		lut->SetTableRange(0, 1);	//值范围，[0, 1]
		lut->SetTableValue(0,				//数值0 映射成 fillColor
			fillColor.GetRed(), fillColor.GetGreen(), fillColor.GetBlue(), 1.0
		);
		lut->SetTableValue(1,				//数值1 映射成 checkerColor
			checkerColor.GetRed(), checkerColor.GetGreen(), checkerColor.GetBlue(), 1.0
		);

		//
		// 为vtkImageData加上颜色查找表
		//
		auto map = vtkSmartPointer<vtkImageMapToColors>::New();
		map->SetLookupTable(lut);
		map->SetOutputFormatToRGBA();
		map->SetInputData(image0);
		map->GetExecutive()->Update();

		//
		// 保存查看
		//
		vtkNew<vtkXMLImageDataWriter> writer;
		writer->SetFileName("[ClipVolume] checkerboard.vti");
		writer->SetInputData(map->GetOutput());
		writer->Write();

		return map->GetOutput();
	}
}