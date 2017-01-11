#include "ui/ui_widget.h"
#include "ui/ui_root.h"

using namespace Halley;

UIWidget::UIWidget(String id, Vector2f minSize, Maybe<UISizer> sizer, Vector4f innerBorder)
	: id(id)
	, size(size)
	, minSize(minSize)
	, innerBorder(innerBorder)
	, sizer(sizer)
{
}

UIWidget::~UIWidget()
{
}

void UIWidget::doDraw(UIPainter& painter) const
{
	draw(painter);
	for (auto& c: getChildren()) {
		c->doDraw(painter);
	}
}

void UIWidget::doUpdate(Time t)
{
	update(t, positionUpdated);
	positionUpdated = false;
}

Vector2f UIWidget::computeMinimumSize() const
{
	Vector2f minSize = getMinimumSize();
	auto sizer = getSizer();
	if (sizer) {
		auto border = getInnerBorder();
		Vector2f innerSize = sizer.get().computeMinimumSize();
		if (innerSize.x > 0.1f || innerSize.y > 0.1f) {
			innerSize += Vector2f(border.x + border.z, border.y + border.w);
		}
		return Vector2f::max(minSize, innerSize);
	}
	return minSize;
}

void UIWidget::setRect(Rect4f rect)
{
	setWidgetRect(rect);
	auto sizer = getSizer();
	if (sizer) {
		auto border = getInnerBorder();
		auto p0 = getPosition();
		sizer.get().setRect(Rect4f(p0 + Vector2f(border.x, border.y), p0 + rect.getSize() - Vector2f(border.z, border.w)));
	}
}

void UIWidget::layout()
{
	Vector2f minimumSize = computeMinimumSize();
	Vector2f targetSize = Vector2f::max(size, minimumSize);
	setRect(Rect4f(getPosition(), getPosition() + targetSize));
}

Maybe<UISizer>& UIWidget::getSizer()
{
	return sizer;
}

const Maybe<UISizer>& UIWidget::getSizer() const
{
	return sizer;
}

bool UIWidget::isFocusable() const
{
	return false;
}

bool UIWidget::isFocused() const
{
	return focused;
}

bool UIWidget::isFocusLocked() const
{
	return false;
}

bool UIWidget::isMouseOver() const
{
	return mouseOver;
}

String UIWidget::getId() const
{
	return id;
}

Vector2f UIWidget::getPosition() const
{
	return position;
}

Vector2f UIWidget::getSize() const
{
	return size;
}

Vector2f UIWidget::getMinimumSize() const
{
	return minSize;
}

Vector4f UIWidget::getInnerBorder() const
{
	return innerBorder;
}

void UIWidget::setPosition(Vector2f pos)
{
	position = pos;
}

void UIWidget::setFocused(bool f)
{
	focused = f;
}

void UIWidget::setMouseOver(bool mo)
{
	mouseOver = mo;
}

void UIWidget::pressMouse(int button)
{
}

void UIWidget::releaseMouse(int button)
{
}

UIRoot& UIWidget::getRoot()
{
	return *uiRoot;
}

void UIWidget::setParent(UIParent& p)
{
	parent = &p;
	uiRoot = &parent->getRoot();
}

void UIWidget::destroy()
{
	if (parent) {
		parent->removeChild(*this);
	}
}

void UIWidget::draw(UIPainter& painter) const
{
}

void UIWidget::update(Time t, bool moved)
{
}

void UIWidget::setWidgetRect(Rect4f rect)
{
	if (position != rect.getTopLeft()) {
		position = rect.getTopLeft();
		positionUpdated = true;
	}
	if (size != rect.getSize()) {
		size = rect.getSize();
		positionUpdated = true;
	}
}