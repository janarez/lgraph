#include "MyForm.h"
#include "GraphLoader.h"
#include <vector>
#include <cmath>
#include <tuple>

// class for loading drawn / file graph
GraphLoader gl;
// keeps all edges for onpaint event
std::vector<std::tuple<int,int,int,int>> edges;
// keeps the l graph solution for onpaint event
std::vector<std::tuple<int, int, int, int,int,int>> solution;

[System::STAThread]
int main(void)
{
	System::Windows::Forms::Application::EnableVisualStyles();
	P1::MyForm form;
	System::Windows::Forms::Application::Run(%form);
	return 0;
}

// creates new vertex
System::Void P1::MyForm::pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	if (radioButton1->Checked)
	{
		++vertexcount;

		// add vertex as custom button
		Vertex^ vertex = gcnew Vertex(vertexcount,e->X, e->Y);
		this->Controls->Add(vertex);
		vertex->BringToFront();

		// add mouse click handler
		vertex->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::button_MouseClick);
				
		// register vertex
		gl.registerVertex(vertexcount);
	}
}

// painting all edges
System::Void P1::MyForm::pictureBox1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	Graphics^ g;
	g = e->Graphics;

	// redraw edges
	Pen^ blackPen = gcnew Pen(Color::Black, 1);
	for (auto const& edge : edges)
	{
		g->DrawLine(blackPen, std::get<0>(edge), std::get<1>(edge), std::get<2>(edge), std::get<3>(edge));
	}
}

// repainting solution
System::Void P1::MyForm::pictureBox2_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	Graphics^ g;
	g = e->Graphics;

	//redraw solution
	Pen^ linePen = gcnew Pen(Color::Black, 2);

	// might interfere if too many vertices are used (35 is still ok, then the form should be enlarged rather than text size)
	System::Drawing::Font^ font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10);
	SolidBrush^ brush = gcnew SolidBrush(Color::Black);

	size_t i = 0;
	for (auto const& l : solution)
	{
		// print shapes ids
		// - 7 so that indeces start above L shape
		g->DrawString(gl.returtVertexID(i + 1).ToString(), font, brush, std::get<0>(l) - 7, 2);

		(i % 2) ? linePen->Color = Color::Black : linePen->Color = Color::Red;
		array<System::Drawing::Point>^ points = {Point(std::get<0>(l),std::get<1>(l)), Point(std::get<2>(l),std::get<3>(l)),Point(std::get<4>(l),std::get<5>(l))};

		g->DrawLines(linePen, points);
		++i;
	}

}

// adding or deleting edges
// deleting vertices
System::Void P1::MyForm::button_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	Graphics^ g;
	g = pictureBox1->CreateGraphics();
	Pen^ blackPen = gcnew Pen(Color::Black, 1);

	Button^ button = safe_cast<Button^>(sender);
	// mark the button
	button->BackColor = Color::Red;
	int x = button->Left;
	int y = button->Top;

	// remembers vertex in case it's the first one for edge handling
	if (first)
		b1 = button;
	else
		b2 = button;

	// delete vertex and its edges
	if (radioButton3->Checked)
	{
		String^ name = button->Name;
		this->Controls->Remove(button);
		// get outgoing edges
		for each (size_t n in gl.returnAdjacent(name))
		{
			// deletes all outgoing edges from edges
			for each (Control^ b in this->Controls->Find(n.ToString(), true))
			{
				int xx = b->Left;
				int yy = b->Top;
				
				// erase edge from edges collection; must check both p1,p2 and p2,p1 edges
				auto it = std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(xx, yy, x, y));
				// check that edge was actually there, then delete
				if (it != edges.end())
					edges.erase(it);
				else
				{
					it = std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(x, y, xx, yy));
					if (it != edges.end())
						edges.erase(it);
				}
			}
		}
		// repaint picturebox1 to remove edges
		pictureBox1->Invalidate();

		// register deletion
		gl.deleteVertex(name);
	}
	// delete edge
	else if (radioButton4->Checked)
	{
		// checked that valid vertex pair is found
		if (!first && b1 != b2)
		{
			// erase edge from edges collection; must check both p1,p2 and p2,p1 edges
			auto it = std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(p1.X, p1.Y, x, y));
			// check that edge was actually there, then delete
			if (it != edges.end())
				edges.erase(it);
			else
			{
				it = std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(x, y, p1.X, p1.Y));
				if (it != edges.end())
					edges.erase(it);
			}
			// repaint picturebox1 to remove edge
			pictureBox1->Invalidate();

			b1->BackColor = Color::Black;
			b2->BackColor = Color::Black;
			first = true;
			// register deleting edge
			gl.deleteEdge(b1->Name, b2->Name);
		}
		// set p1 to marked vertex location
		else
		{
			p1 = Point(x, y);
			first = false;
		}

	}
	// add edge
	else if (radioButton2->Checked)
	{
		// second vertex must be different from first one
		if (!first && b1 != b2)
		{
			p2 = Point(x, y);
			g->DrawLine(blackPen, p1, p2);

			// only add each edge once
			if (std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(p1.X, p1.Y, x, y)) == edges.end() &&
				std::find(edges.begin(), edges.end(), std::tuple<int, int, int, int>(x, y, p1.X, p1.Y)) == edges.end())
			{
				edges.push_back(std::tuple<int, int, int, int>(p1.X, p1.Y, x, y));
			}

			b1->BackColor = Color::Black;
			b2->BackColor = Color::Black;
			first = true;

			// register edge
			gl.registerEdge(b1->Name, b2->Name);
		}
		else
		{
			p1 = Point(x, y);
			first = false;
		}
	}
}

// calculating l graph
// also shows result in picturebox2
System::Void P1::MyForm::button2_Click(System::Object^  sender, System::EventArgs^  e)
{
	// clear previous solution
	solution.clear();
	// make sure output window is empty -> whiten l graph picturebox
	pictureBox2->Image = nullptr;
	pictureBox2->Refresh();

	
	// set vertex count in graphloader
	gl.setVertexCount(vertexcount);
	// remove zero degree vertices form neighbors
	std::map<size_t, std::set<size_t>> neighbors = gl.removeZeroDegreeVertices();

	// find solution if it exists
	bool exists = gl.permuteNeighbors();
	
	Graphics^ g;
	g = pictureBox2->CreateGraphics();
	
	// draw solution
	if (exists)
	{
		int width = pictureBox2->Width;
		int heigth = pictureBox2->Height;
		size_t count = gl.igraph.shapes.size() -1;

		// parameters for distance between l shapes
		int stepUp = heigth / (count + 1);
		int stepAcross = width / (count + 1);
		Pen^ linePen = gcnew Pen(Color::BlueViolet, 2);
		// might interfere if too many vertices are used (35 is still ok, then the form should be enlarged rather than text size)
		System::Drawing::Font^ font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10);
		SolidBrush^ brush = gcnew SolidBrush(Color::Black);

		// draw l shapes one by one
		for (size_t i = 0; i < count; ++i)
		{
			size_t _p1 = gl.igraph.shapes[i].getUp();
			size_t _p2 = gl.igraph.shapes[i].getBend();
			size_t _p3 = gl.igraph.shapes[i].getSide();

			// - 7 so that indeces start above L shape
			g->DrawString(gl.returtVertexID(i+1).ToString(), font ,brush, _p1*stepAcross-7, 2);

			// 17 is pixel offset for text with indices
			size_t offset = 17;

			Point p0 = Point(_p1 * stepAcross, offset);
			Point p1 = Point(_p1 * stepAcross, offset + 0.5*stepUp);
			Point p2 = Point(_p1 * stepAcross, offset + _p2 * stepUp);
			// returnDirection() to make lines cross each other, not just touch
			Point p3 = Point(_p3 * stepAcross + 0.5 * stepAcross * gl.igraph.returnDirection(i), 17 + _p2 * stepUp);

			(i % 2) ? linePen->Color = Color::Black : linePen->Color = Color::Red;

			// insert shape into solution for redrawing
			solution.push_back(std::tuple<int,int,int,int,int,int>(p0.X, p0.Y, p2.X, p2.Y, p3.X, p3.Y));
			array<System::Drawing::Point>^ points = { p0,p1,p2,p3 };
			g->DrawLines(linePen, points);
		}
		return;
	}
	// no solution found
	else
	{
		String^ string = "L Representation does not exist";
		System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial", 16);
		SolidBrush^ brush = gcnew SolidBrush(Color::Black);
		PointF point = PointF(10, 10);
		g->DrawString(string, font, brush, point);
	}
}

// loading graph from file
System::Void P1::MyForm::button1_Click(System::Object^  sender, System::EventArgs^  e)
{
	OpenFileDialog^ ofd = gcnew OpenFileDialog;
	ofd->ShowDialog();
	String^ file = ofd->FileName;
	// clear all clicked vertices
	button3_Click(sender,e);
	gl.loading(file);

	// must set vertexcount here, so that setVertexIDs works
	vertexcount = gl.neighbors.rbegin()->first;
	drawLoadedGraph();
	// redraw the control to see edges
	pictureBox1->Invalidate();

	delete ofd;
}

// draws loaded graph as n-sided regular polygon
System::Void P1::MyForm::drawLoadedGraph(void)
{
	// calculate polygon centre
	int mid_x = pictureBox1->Width / 2;
	int mid_y = pictureBox2->Height / 2;
	float r = (mid_x < mid_y) ? 0.8 * mid_x : 0.8 * mid_y;
	float angle = 2 * pi / vertexcount;

	// store coordinates
	std::vector<int> x(vertexcount+1);
	std::vector<int> y(vertexcount+1);

	// calculate coordinates of every vertex
	for (size_t v = 1; v <= vertexcount; ++v)
	{
		x[v] = (int)(mid_x + r * cos(angle * v));
		y[v] = (int)(mid_y + r * sin(angle * v));
	}

	// create vertices and draw the edges
	for (size_t v = 1; v <= vertexcount; ++v)
	{
		// add vertex
		Vertex^ vertex = gcnew Vertex(v, x[v], y[v]);
		this->Controls->Add(vertex);
		vertex->BringToFront();

		// add mouse click handler
		vertex->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::button_MouseClick);

		// register all edges (will be painted after return)
		for (auto u : gl.neighbors[v])
		{
			// register only once
			if(u < v)
				edges.push_back(std::tuple<int,int,int,int>(x[u], y[u], x[v], y[v]));
		}
	}
}

// clear all that was done so far
// deletes all drawn and loaded
System::Void P1::MyForm::button3_Click(System::Object^  sender, System::EventArgs^  e)
{
	// erase loaded graph
	gl.neighbors.clear();
	gl.components.clear();

	// delete vertices
	for (size_t i = 1; i <= vertexcount; ++i)
	{
		for each (Control^ b in this->Controls->Find(i.ToString(), true))
		{
			this->Controls->Remove(b);
		}
	}

	// reset all form variables
	vertexcount = 0;
	first = true;
	edges.clear();
	solution.clear();

	// whiten l graph picturebox
	pictureBox2->Image = nullptr;
	pictureBox2->Refresh();
	// whiten input picturebox
	pictureBox1->Image = nullptr;
	pictureBox1->Refresh();
}

// saves graph to file
System::Void P1::MyForm::button4_Click(System::Object^  sender, System::EventArgs^  e)
{
	SaveFileDialog^ sfd = gcnew SaveFileDialog;
	sfd->ShowDialog();

	String^ filename = sfd->FileName;

	System::IO::StreamWriter^ fout = gcnew System::IO::StreamWriter(filename);

	// generate all edge pairs uv, where u < v
	// format: each line is: u v
	for (auto const& i : gl.neighbors)
	{
		for (auto v : i.second)
		{
			if (i.first < v)
			{
				String^ edge = i.first + " " + v;
				fout->Write(edge);
				fout->WriteLine();
			}
		}
	}

	fout->Close();

	delete fout;
	delete sfd;
}