#include "Graphics.h"
#include "Solver.h"

void Graphics::init(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	if (FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
	{
		throw - 1;
	}
	else if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pWriteFactory)
	))) {
		throw - 1;
	}

}

void Graphics::onPaint()
{
	// Ensure resources are created
	createResources();

	// Prepare window for painting
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	pRenderTarget->BeginDraw();

	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw UI

	// Draw soduku grid
	drawSudoku(*grid);

	// End paint
	pRenderTarget->EndDraw();

	EndPaint(m_hWnd, &ps);
}

void Graphics::onResize()
{
	// Resize
	GetClientRect(m_hWnd, &winRect);
	destroyResources();
	
	// Then repaint window
	onPaint();
}

void Graphics::createResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		D2D1_SIZE_U size = D2D1::SizeU(winRect.right, winRect.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&pRenderTarget);

		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlack);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &pGrey);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &pBlue);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pBackground);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &pHighlight);

		if (size.width == 0 || size.height == 0) {
			size.width = 100;
			size.height = 100;
		}

		pWriteFactory->CreateTextFormat(
			L"Arial",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			size.height / SUDOKU_SIZE / 2 * 96.0f / 72.0f,
			L"en-US",
			&pText
		);

		// Center align (horizontally) the text.
		HRESULT hr = pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		if (SUCCEEDED(hr))
		{
			hr = pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}

	}
}

void Graphics::destroyResources()
{
	SafeRelease(&pRenderTarget);

	SafeRelease(&pBlack);
	SafeRelease(&pGrey);
	SafeRelease(&pBlue);
	SafeRelease(&pBackground);
	SafeRelease(&pHighlight);

	SafeRelease(&pText);
}

void Graphics::drawSudoku(const Sudoku& s) const{

	int sudokuSideLength = (int) round(winRect.bottom * 3 / 4 / s.size) * s.size;
	int xOffset = (winRect.right - sudokuSideLength) / 2;
	int yOffset = (winRect.bottom - sudokuSideLength - 100) / 2;
	
	float columnWidth = sudokuSideLength / s.size;
	float rowHeight = columnWidth;

	for (int x = 0; x < s.size; x++) {
		for (int y = 0; y < s.size; y++) {
			int& cell = s.grid[x][y];
			if (cell != -1) {
				if (cell == -2) {
					//Fill cell black (or dark grey so gridlines remain visible)
					pRenderTarget->FillRectangle(
						D2D1::RectF(xOffset + x * columnWidth, yOffset + y * rowHeight, xOffset + (x + 1) * columnWidth, yOffset + (y + 1) * rowHeight),
						pGrey);
				}
				else if (cell < SUDOKU_SIZE + 1) {
					// Draw the correct digit into the cell in colour 1

					std::wstring text = std::to_wstring(cell);

					pRenderTarget->DrawText(
						text.c_str(),
						text.size(),
						pText,
						D2D1::RectF(xOffset + x * columnWidth, yOffset + y * rowHeight, xOffset + (x + 1) * columnWidth, yOffset + (y + 1) * rowHeight),
						pBlack
					);
				}
				else {
					// Draw the correct digit into the cell in colour 2
					std::wstring text = std::to_wstring(cell - SUDOKU_SIZE - 1);

					pRenderTarget->DrawText(
						text.c_str(),
						text.size(),
						pText,
						D2D1::RectF(xOffset + x * columnWidth, yOffset + y * rowHeight, xOffset + (x + 1) * columnWidth, yOffset + (y + 1) * rowHeight),
						pBlue
					);
				}
			}
		}
	}

	for (int x = 0; x < s.size + 1; x++) {
		if (x % s.boxWidth == 0) {
			// Draw thick vertical line
			pRenderTarget->DrawLine(
				D2D1::Point2F(xOffset + x * columnWidth, yOffset),
				D2D1::Point2F(xOffset + x * columnWidth, winRect.bottom - yOffset - 100),
				pBlack, 3);
		}
		else {
			// Draw narrow vertical line
			pRenderTarget->DrawLine(
				D2D1::Point2F(xOffset + x * columnWidth, yOffset),
				D2D1::Point2F(xOffset + x * columnWidth, winRect.bottom - yOffset - 100),
				pBlack, 1);
		}
	}

	for (int y = 0; y < s.size + 1; y++) {
		if (y % s.boxHeight == 0) {
			// Draw thick horizontal line
			pRenderTarget->DrawLine(
				D2D1::Point2F(xOffset, yOffset + y * rowHeight),
				D2D1::Point2F(winRect.right - xOffset, yOffset + y * rowHeight),
				pBlack, 3);
		}
		else {
			// Draw narrow horizontal line
			pRenderTarget->DrawLine(
				D2D1::Point2F(xOffset, yOffset + y * rowHeight),
				D2D1::Point2F(winRect.right - xOffset, yOffset + y * rowHeight),
				pBlack, 1);
		}
	}

	// Draw a box to highlight the cell specified in highlightedCell. This could be easily modified to allow the highlighting of multiple cells.
	if (currentCell.x > -1 && currentCell.y > -1) {
		pRenderTarget->DrawRectangle(
			D2D1::RectF(
				xOffset + currentCell.x * columnWidth + 3, yOffset + currentCell.y * rowHeight + 3,
				xOffset + (currentCell.x + 1) * columnWidth - 3, yOffset + (currentCell.y + 1) * rowHeight - 3
			),
			pHighlight, 3);
	}
}
