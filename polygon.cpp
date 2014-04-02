#include "polygon.h"

sectionFuncReturn::sectionFuncReturn(vector<Point> newPoints,
									 vector<ushort> morePoints,
									 vector<ushort> lessPoints,
									 vector<ushort> onPoints)
{
	newPoints_ = newPoints;
	morePoints_ = morePoints;
	lessPoints_ = lessPoints;
	onPoints_ = onPoints;
}


Polygon::Polygon(array<float, 4> color)
{
	color_ = color;
}

Polygon::Polygon(vector<Point> vertexes, array<float, 4> color)
{
	if(vertexes.size() > 2)
		vertexes_ = vertexes;
	color_ = color;
}

Polygon::Polygon(list<Point> vertexes, array<float, 4> color)
{
	if(vertexes.size() > 2)
		for(auto i = vertexes.begin(); i != vertexes.end(); ++i )
			vertexes_.push_back(*i);
	color_ = color;
}

Polygon& Polygon::operator = (Polygon const & polygon)
{
	vertexes_ = polygon.vertexes_;
	return *this;
}

void Polygon::clear()
{
	vertexes_.clear();
}

inline void Polygon::addVertex(const Point &vertex)
{
	vertexes_.push_back(vertex);
}

void Polygon::addVertexesList(const list<Point> &vertexes)
{
	for(list<Point>::const_iterator i = vertexes.begin() ; i != vertexes.end(); ++i)
		vertexes_.push_back(*i);
}

void Polygon::draw()
{

	//glPolygonMode();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);

	glColor4f(0.20f, 0.20f, 1.00f, 0.5f);
	for(ushort i=0; i<vertexes_.size(); ++i)
		glVertex3f(vertexes_[i][0], vertexes_[i][1], vertexes_[i][2]);
	glEnd();
	/*
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
	glStraightLineWidth(3);

	glColor4f(0.00f, 0.00f, 0.00f, 1.0f);
	for(ushort i=0; i<this->vertexCount_; ++i)
		glVertex3f(vertexes_[i][0], vertexes_[i][1], vertexes_[i][2]);
	glEnd();

*/
	glLineWidth(3);
	glColor4f(0.00f, 0.00f, 0.00f, 0.0f);
	for(ushort i=0; i<vertexes_.size()-1; ++i)
	{
		glBegin(GL_LINES);
		glVertex3f(vertexes_[i][0], vertexes_[i][1], vertexes_[i][2]);
		glVertex3f(vertexes_[i+1][0], vertexes_[i+1][1], vertexes_[i+1][2]);
		glEnd();
	}
	glBegin(GL_LINES);
	glVertex3f(vertexes_[vertexes_.size()-1][0], vertexes_[vertexes_.size()-1][1], vertexes_[vertexes_.size()-1][2]);
	glVertex3f(vertexes_[0][0], vertexes_[0][1], vertexes_[0][2]);
	glEnd();


}


//
bool Polygon::polygonSection(Plane plane, Polygon* polygonPointer1, Polygon* polygonPointer2, list<Point>* sectionStraightLine)
{
	ushort temp = 0;
	schar currentRelativePosition = 0;
	vector<schar> pointsProvisionVector, pointsAccessoryVector;
	pointsProvisionVector.resize(vertexes_.size(), 0);
	pointsAccessoryVector.resize(vertexes_.size(), 0);

	list<Point> newPolygonsVertexes[2];

	Point bufPoint;

	for(ushort i=0; i<vertexes_.size(); ++i)
	{
		pointsProvisionVector[i] = pointProvisionRelativeToPlane(vertexes_[i], plane);

		currentRelativePosition = pointsProvisionVector[i]*((currentRelativePosition == 0) ||
												  (pointsProvisionVector[i] == -currentRelativePosition)) +
								  currentRelativePosition*(pointsProvisionVector[i] != -currentRelativePosition);
		pointsAccessoryVector[i] = currentRelativePosition;
	}

	for(ushort i=0; pointsProvisionVector[i] == 0 && i < vertexes_.size(); ++i)
	{
		currentRelativePosition = pointsProvisionVector[i]*((currentRelativePosition == 0) ||
												  (pointsProvisionVector[i] == -currentRelativePosition)) +
								  currentRelativePosition*(pointsProvisionVector[i] != -currentRelativePosition);
		pointsAccessoryVector[i] = currentRelativePosition;
	}


	for(ushort i=0; i<vertexes_.size(); ++i)
	{
		if(pointsProvisionVector[i]*pointsProvisionVector[
				temp = (i-1+pointsProvisionVector.size())%pointsProvisionVector.size() ] == -1)
		{
			planeCrossingByPiece(plane,
								 Piece(vertexes_[temp], vertexes_[i]),
								 &bufPoint);
			newPolygonsVertexes[0].push_back(bufPoint);
			newPolygonsVertexes[1].push_back(bufPoint);
			if(sectionStraightLine != 0)
				sectionStraightLine->push_back(bufPoint);

		}
		if((pointsProvisionVector[temp] == 0) && (pointsAccessoryVector[temp] != pointsAccessoryVector[i]))
		{
			newPolygonsVertexes[1 - (pointsAccessoryVector[temp]*(-1) < 0)].push_back(vertexes_[temp]);
			if(sectionStraightLine != 0)
				sectionStraightLine->push_back(vertexes_[temp]);
		}

		newPolygonsVertexes[1 - (pointsAccessoryVector[i] < 0)].push_back(vertexes_[i]);
	}
	if( sectionStraightLine != 0 &&
	   (newPolygonsVertexes[0].size() > 2) &&
	   (newPolygonsVertexes[1].size() > 2) )
		sortPointsOnStraightLine(sectionStraightLine,
								 sectionStraightLine->front()-(*(++(sectionStraightLine->begin()))));



	if(polygonPointer1 != 0 && newPolygonsVertexes[0].size() > 2)
		polygonPointer1->addVertexesList(newPolygonsVertexes[0]);
	if(polygonPointer2 != 0 && newPolygonsVertexes[1].size() > 2)
		polygonPointer2->addVertexesList(newPolygonsVertexes[1]);
	return (newPolygonsVertexes[0].size() > 2) && (newPolygonsVertexes[1].size() > 2);


}