// Fill out your copyright notice in the Description page of Project Settings.

#include "VisionHUD.h"

AVisionHUD::AVisionHUD()
{
	// Get Minimap render target from blueprint
	static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> MiniMapTexObj(TEXT("/Game/T_Minimap"));
	MiniMapTextureRenderTarget = MiniMapTexObj.Object;

	// Creates Texture2D to store MiniMapTex content
	MiniMapTexture = UTexture2D::CreateTransient(MiniMapTextureRenderTarget->SizeX, MiniMapTextureRenderTarget->SizeY, PF_B8G8R8A8);

#if WITH_EDITORONLY_DATA
	MiniMapTexture->MipGenSettings = TMGS_NoMipmaps;
#endif
	MiniMapTexture->SRGB = MiniMapTextureRenderTarget->SRGB;

	MiniMapRenderTarget = MiniMapTextureRenderTarget->GameThread_GetRenderTargetResource();

	wrotefile = false;
}

AVisionHUD::~AVisionHUD()
{
}

void AVisionHUD::DrawHUD()
{

	Super::DrawHUD();

	SaveRenderTargetToDisk(MiniMapTextureRenderTarget, "c:\\Users\\levys\\Desktop\\image.bmp");

	

	// Adapted from https://answers.unrealengine.com/questions/193827/how-to-get-texture-pixels-using-utexturerendertarg.html

	// Read the pixels from the RenderTarget and store them in a FColor array
	MiniMapRenderTarget->ReadPixels(MiniMapSurfData);

	for (int x = 0; x < MiniMapTextureRenderTarget->SizeX; ++x) {

		for (int y = 0; y < MiniMapTextureRenderTarget->SizeY; ++y) {

			int k = x + y * MiniMapTextureRenderTarget->SizeX;

			FColor PixelColor = MiniMapSurfData[k];

			DrawRect(PixelColor, x, y, 1, 1);


			// RGB->gray formula from https ://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
			//imgdata[k] = (byte)(0.21 *PixelColor.R + 0.72 * PixelColor.G + 0.07 * PixelColor.B);
		}
	}

/*

	int rows = MiniMapTextureRenderTarget->SizeY;
	int cols = MiniMapTextureRenderTarget->SizeX;

	float rightx = LEFTX + WIDTH;
	float bottomy = TOPY + HEIGHT;

	drawBorder(LEFTX, TOPY, rightx, TOPY);
	drawBorder(rightx, TOPY, rightx, bottomy);
	drawBorder(rightx, bottomy, LEFTX, bottomy);
	drawBorder(LEFTX, bottomy, LEFTX, TOPY);*/
}

void AVisionHUD::drawBorder(float lx, float uy, float rx, float by)
{

	DrawLine(lx, uy, rx, by, BORDER_COLOR, BORDER_WIDTH);
}

void AVisionHUD::SaveRenderTargetToDisk(UTextureRenderTarget2D* InRenderTarget, FString Filename)
{
	FTextureRenderTargetResource* RTResource = InRenderTarget->GameThread_GetRenderTargetResource();

	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	ReadPixelFlags.SetLinearToGamma(true);

	TArray<FColor> OutBMP;
	RTResource->ReadPixels(OutBMP, ReadPixelFlags);

	for (FColor& color : OutBMP)
	{
		color.A = 255;
	}


	FIntRect SourceRect;

	FIntPoint DestSize(InRenderTarget->GetSurfaceWidth(), InRenderTarget->GetSurfaceHeight());

	FString ResultPath;
	FHighResScreenshotConfig& HighResScreenshotConfig = GetHighResScreenshotConfig();
	HighResScreenshotConfig.SaveImage(Filename, OutBMP, DestSize, &ResultPath);
}

