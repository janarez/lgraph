#include "MyForm.h"
#include "LIntersect.h"
#include "GraphLoader.h"
#include <vector>


GraphLoader gl;

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
		Button^ button = gcnew System::Windows::Forms::Button();
		this->Controls->Add(button);
		button->Location = System::Drawing::Point(e->X, e->Y);
		button->BackColor = Color::Black;
		button->Size = System::Drawing::Size(20, 20);
		button->ForeColor = Color::White;
		button->Text = vertexcount.ToString();
		button->Font = gcnew System::Drawing::Font("Microsoft Sans Serif",7);
		button->BringToFront();
		button->FlatStyle = FlatStyle::Flat;
		button->FlatAppearance->BorderSize = 0;
		button->Name = vertexcount.ToString();
		button->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::button_MouseClick);
		
		// register vertex
		gl.registerVertex(vertexcount);
	}
}

// adding or deleting edges
// deleting vertices
System::Void P1::MyForm::button_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	Graphics^ g;
	g = pictureBox1->CreateGraphics();
	Pen^ blackPen = gcnew Pen(Color::Black, 1);
	Pen^ whitePen = gcnew Pen(Color::White, 1);

	Button^ button = safe_cast<Button^>(sender);
	button->BackColor = Color::Red;
	int x = button->Left;
	int y = button->Top;

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
			for each (Control^ b in this->Controls->Find(n.ToString(), true))
			{
				int xx = b->Left;
				int yy = b->Top;
				p2 = Point(xx, yy);
				g->DrawLine(whitePen, Point(x, y), p2);
			}
		}

		// register deletion
		gl.deleteVertex(name);
	}
	// delete edge
	else if (radioButton4->Checked)
	{
		if (!first)
		{
			p2 = Point(x, y);
			g->DrawLine(whitePen, p1, p2);
			b1->BackColor = Color::Black;
			b2->BackColor = Color::Black;
			first = true;
			// register deleting edge
			gl.deleteEdge(b1->Name, b2->Name);
		}
		else
		{
			p1 = Point(x, y);
			first = false;
		}

	}
	// add edge
	else if (radioButton2->Checked)
	{
		if (!first)
		{
			p2 = Point(x, y);
			g->DrawLine(blackPen, p1, p2);
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
	// set vertex count in graphloader
	gl.setVertexCount(vertexcount);
	// do magic :)
	bool exists = false;
	LIntersectionGraph igraph(gl.removeZeroDegreeVertices());
	bool first = true;
	do
	{
		if (first)
		{
			exists = igraph.createLGraph();
			first = false;
		}
		else
		{
			try
			{
				igraph.neighbors = gl.permuteNeighbors();
			}
			catch (const std::exception&)
			{
				break;
			}
			exists = igraph.createLGraph();
		}
	} while (!exists);
	
	Graphics^ g;
	g = pictureBox2->CreateGraphics();

	// make sure output window is empty -> whiten l graph picturebox
	pictureBox2->Image = nullptr;
	pictureBox2->Refresh();
	
	if (exists)
	{
		int width = pictureBox2->Width;
		int heigth = pictureBox2->Height;
		// fixed in cum branch, might need fixing because indexing was changed
		size_t count = igraph.shapes.size() -1;

		int stepUp = heigth / (count + 1);
		int stepAcross = width / (count + 1);
		Pen^ linePen = gcnew Pen(Color::BlueViolet, 2);
		// might interfere if too many vertices are used (35 is still ok, then the form should be enlarged rather than text size)
		System::Drawing::Font^ font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10);
		SolidBrush^ brush = gcnew SolidBrush(Color::Black);

		for (size_t i = 0; i < count; ++i)
		{
			size_t _p1 = igraph.shapes[i].getUp();
			size_t _p2 = igraph.shapes[i].getBend();
			size_t _p3 = igraph.shapes[i].getSide();

			// - 8 so that indeces start above L shape
			g->DrawString(gl.returtVertexID(i+1).ToString(), font ,brush, _p1*stepAcross-7, 2);

			// 17 is pixel offset for text with indices
			Point p0 = Point(_p1 * stepAcross, 17);
			Point p1 = Point(_p1 * stepAcross, 17 + 0.5*stepUp);
			Point p2 = Point(_p1 * stepAcross, 17 + _p2 * stepUp);
			Point p3 = Point(_p3 * stepAcross + 0.5 * stepAcross * igraph.returnDirection(i), 17 + _p2 * stepUp);

			(i % 2) ? linePen->Color = Color::Black : linePen->Color = Color::Red;

			array<System::Drawing::Point>^ points = { p0,p1,p2,p3 };
			g->DrawLines(linePen, points);
		}
		return;
	}
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
	// clear all
	button3_Click(sender,e);
	gl.loading(file);

	// display that graph file has been loaded
	String^ string = "Graph file is loaded";
	System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial", 16);
	SolidBrush^ brush = gcnew SolidBrush(Color::Black);
	PointF point = PointF(10, 10);
	Graphics^ g;
	g = pictureBox1->CreateGraphics();
	g->DrawString(string, font, brush, point);

	// must set vertexcount here, so that setVertexIDs works
	vertexcount = gl.neighbors.size();
}

// clear all that was done so far
// deletes all drawn and loaded
System::Void P1::MyForm::button3_Click(System::Object^  sender, System::EventArgs^  e)
{
	// erase loaded graph
	gl.neighbors.clear();
	// whiten l graph picturebox
	pictureBox2->Image = nullptr;
	pictureBox2->Refresh();
	// whiten input picturebox
	pictureBox1->Image = nullptr;
	pictureBox1->Refresh();
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
}