// 
// C++ for C Programmers Part 1
// Week 3 Peer Graded Assignment Homework
//
// Dijkstra's Algorithm
// 
// Connectivity Matrices
// 
// Output shown at end of code.
// 
// *************************************************
// * Results of Randomly Generated City Graphs*
// * size | density | avg dist *
// *50 | 20 | 6.39 *
// *50 | 40 | 4.75 *
// *************************************************
//
// Prashant Kumar
// Mar 12th 2022
// 

#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <time.h>

using namespace std;

struct Neighbor
{
	int index = -1;
	int nearest_neighbor_index = -1;
	float distance = -1;
	//constructor
	Neighbor(int index, float distance)
	{
		this->index = index;
		this->nearest_neighbor_index = -1;
		this->distance = distance;
	}
	//constructor
	Neighbor(int index, int nearest_neighbor_index, float distance)
	{
		this->index = index;
		this->nearest_neighbor_index = nearest_neighbor_index;
		this->distance = distance;
	}
};

class CityGraph 
{
	bool** city_connectivity_matrix_;
	float** city_distance_matrix_;
	int density_ = 0;
	int size_ = -1;
	const int kmax_print_columns_to_show_per_page_ = 13;
	void InitializeCityMatrices()
	{
		//heap created 2D array of graph
		//allocate row pointers
		city_connectivity_matrix_ = new bool* [size_];
		city_distance_matrix_ = new float* [size_];
		//allocate column pointers
		for (int i = 0; i < size_; i++) {
			city_connectivity_matrix_[i] = new bool[size_];
			city_distance_matrix_[i] = new float[size_];
		}
		//initialize default values
		for (int i = 0; i < size_; i++)
			for (int j = 0; j < size_; j++)
			{
				city_connectivity_matrix_[i][j] = false;
				city_distance_matrix_[i][j] = 0;
			}
	}
	void GenerateConnectivityAndDistanceMatrix() 
	{
		InitializeCityMatrices();
		srand(static_cast<unsigned int>(time(0))); //seed time
		for (int i = 0; i < size_; i++)
			for (int j = 0; j < size_; j++)
				if (i != j) 
				{
					city_connectivity_matrix_[i][j] = city_connectivity_matrix_[j][i] = (rand() % 100 <= density_ ? true : false);
					if (city_connectivity_matrix_[i][j])
					{
						//give random distance values between 1.0 to 10.0
						int randVal = rand() / 100;
						int randMax = RAND_MAX / 100;
						city_distance_matrix_[i][j] = city_distance_matrix_[j][i] = static_cast<float>(90 * randVal / randMax) / 10 + 1;
					}
				}
	}
public:
	float avg_dist_ = 0;
	//test dataset constructor
	CityGraph()
	{
		this->size_ = 9;
		this->density_ = 0;
		cout << "Test Data City Graph with size " << size_ << endl << endl;
		InitializeCityMatrices();
		//test dataset for half matrix
		cout << "Implementing graph shown here https ://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/" << endl << endl;
		city_connectivity_matrix_[0][1] = true;
		city_connectivity_matrix_[0][7] = true;
		city_connectivity_matrix_[1][2] = true;
		city_connectivity_matrix_[1][7] = true;
		city_connectivity_matrix_[7][8] = true;
		city_connectivity_matrix_[2][3] = true;
		city_connectivity_matrix_[2][8] = true;
		city_connectivity_matrix_[2][5] = true;
		city_connectivity_matrix_[6][7] = true;
		city_connectivity_matrix_[6][8] = true;
		city_connectivity_matrix_[5][6] = true;
		city_connectivity_matrix_[3][4] = true;
		city_connectivity_matrix_[3][5] = true;
		city_connectivity_matrix_[4][5] = true;
		city_distance_matrix_[0][1] = 4;
		city_distance_matrix_[0][7] = 8;
		city_distance_matrix_[1][2] = 8;
		city_distance_matrix_[1][7] = 11;
		city_distance_matrix_[7][8] = 7;
		city_distance_matrix_[2][3] = 7;
		city_distance_matrix_[2][8] = 2;
		city_distance_matrix_[2][5] = 4;
		city_distance_matrix_[6][7] = 1;
		city_distance_matrix_[6][8] = 6;
		city_distance_matrix_[5][6] = 2;
		city_distance_matrix_[3][4] = 9;
		city_distance_matrix_[3][5] = 14;
		city_distance_matrix_[4][5] = 10;
		//mirror matrices across diagnal
		for (int i = 0; i < size_; i++)
			for (int j = 0; j < size_; j++)
				if (i < j) 
				{
					city_connectivity_matrix_[j][i] = city_connectivity_matrix_[i][j];
					city_distance_matrix_[j][i] = city_distance_matrix_[i][j];
				}
		//print connectivity matrix
		PrintCityConnectivityMatrix();
		PrintCityDistanceMatrix();
		//call avg distance to origin city method
		AvgPathLengthUsingDijkstrasAlgorithm();
	}
	//constructor
	CityGraph(int size_, int density_) 
	{
		this->size_ = size_;
		this->density_ = density_;
		//draw a randomly generated Connectivity Matrix
		GenerateConnectivityAndDistanceMatrix();
		cout << "Generated City Graph with size " << size_ << " and density " << density_ << endl << endl;
		//print connectivity matrix
		PrintCityConnectivityMatrix();
		PrintCityDistanceMatrix();
		//call avg distance to origin city method
		AvgPathLengthUsingDijkstrasAlgorithm();
	}
	//destructor
	~CityGraph() 
	{
		for (int i = 0; i < size_; i++) 
		{
			delete[] city_connectivity_matrix_[i];
			delete[] city_distance_matrix_[i];
		}
		delete[] city_connectivity_matrix_;
		delete[] city_distance_matrix_;
		cout << "City Graph with size " << size_;
		if(density_ > 0) cout << " and density " << density_;
		cout << " deleted.\n" << endl;
	}
	void PrintCityConnectivityMatrix()
	{
		int pages = size_ / kmax_print_columns_to_show_per_page_;
		for (int p = 0; p <= pages; p++)
		{
			cout << "Edges:\t";
			for (int i = p * kmax_print_columns_to_show_per_page_; i < min(size_ , (p + 1) * kmax_print_columns_to_show_per_page_); i++)
				cout << "|   " << i << "\t";
			cout << endl << "--------";
			for (int i = p * kmax_print_columns_to_show_per_page_; i < min(size_, (p + 1) * kmax_print_columns_to_show_per_page_); i++)
				cout << "|-------";
			cout << endl;
			for (int i = 0; i < size_; i++) 
			{
				cout << "   " << i << "\t";
				for (int j = p * kmax_print_columns_to_show_per_page_; j < min(size_, (p + 1) * kmax_print_columns_to_show_per_page_); j++)
					if (i != j)
						cout << "|    " << city_connectivity_matrix_[i][j] << "\t";
					else
						cout << "|    -\t";
				cout << endl;
			}
			cout << endl;
		}
	}
	void PrintCityDistanceMatrix()
	{
		int pages = size_ / kmax_print_columns_to_show_per_page_;
		for (int p = 0; p <= pages; p++)
		{
			cout << "Dist:\t";
			for (int i = p * kmax_print_columns_to_show_per_page_; i < min(size_, (p + 1) * kmax_print_columns_to_show_per_page_); i++)
				cout << "|   " << i << "\t";
			cout << endl << "--------";
			for (int i = p * kmax_print_columns_to_show_per_page_; i < min(size_, (p + 1) * kmax_print_columns_to_show_per_page_); i++)
				cout << "|-------";
			cout << endl;
			for (int i = 0; i < size_; i++) 
			{
				cout << "   " << i << "\t";
				for (int j = p * kmax_print_columns_to_show_per_page_; j < min(size_, (p + 1) * kmax_print_columns_to_show_per_page_); j++) 
				{
					cout << "|  ";
					if (i == j)
						printf("  -");
					else if (city_distance_matrix_[i][j] != 0)
						if (city_distance_matrix_[i][j] >= 10)
							printf("%0.1f", city_distance_matrix_[i][j]);
						else
							printf("%0.1f ", city_distance_matrix_[i][j]);
					else
						printf("  0");
					cout << "\t";
				}
				cout << endl;
			}
			cout << endl;
		}
	}
	int get_density_()
	{
		return this->density_;
	}
	int get_size_()
	{
		return this->size_;
	}
	vector<Neighbor> GetNeighbors(int cityIndex)
	{
		vector<Neighbor> neighborsList;
		for (int i = 0; i < size_; i++)
		{
			if (city_connectivity_matrix_[cityIndex][i])
			{
				Neighbor neighbor = Neighbor(i, city_distance_matrix_[cityIndex][i]);
				neighborsList.push_back(neighbor);
			}
		}
		return neighborsList;
	}
	float GetNeighborDistance(int cityIndex, int neighborIndex)
	{
		return city_distance_matrix_[cityIndex][neighborIndex];
	}
	void AvgPathLengthUsingDijkstrasAlgorithm();
};

template <typename T>
void PrintOpenSet(T p)
{
	T q = p;
	cout << "Open Set: ";
	while (!q.empty())
	{
		cout << '\t' << static_cast<Neighbor>(q.top()).distance;
		q.pop();
	}
	cout << endl;
}

void CityGraph::AvgPathLengthUsingDijkstrasAlgorithm()
{
	//define closed and open sets
	vector<Neighbor> closed_set;
	//using priority_queue to learn how to use it
	auto cmpFn = [](Neighbor left, Neighbor right) {return left.distance > right.distance; };
	priority_queue<Neighbor, vector<Neighbor>, decltype(cmpFn)> open_set(cmpFn);

	//dijkstras algorithm
	//add origin city to closed set
	closed_set.push_back(Neighbor(0, 0));
	//add neighbors of origin city to open set
	vector<Neighbor> current_neighbors = this->GetNeighbors(0);
	for (Neighbor neighbor_city : current_neighbors)
	{
		neighbor_city.nearest_neighbor_index = 0;
		open_set.push(neighbor_city);
	}
	while (open_set.size() > 0)
	{
		//move nearest city, which is top of open set, to closed set. Call it current city.
		Neighbor current_city = open_set.top();
		open_set.pop();
		closed_set.push_back(current_city);
		//for each neighbor city of current city which is not in closed set
		current_neighbors = this->GetNeighbors(current_city.index);
		for (Neighbor neighbor_city : current_neighbors)
		{
			bool found_neighbor_city_in_closed_set = false;
			for (auto iterator : closed_set)
				if (iterator.index == neighbor_city.index)
				{
					found_neighbor_city_in_closed_set = true;
					break;
				}
			if (!found_neighbor_city_in_closed_set)
			{
				//if neighbor city is in open set then update its distance in open set
				bool found_neighbor_city_in_open_set = false;
				vector<Neighbor> open_set_temp;
				while (!open_set.empty())
				{
					Neighbor open_set_top = open_set.top();
					if (open_set_top.index == neighbor_city.index)
					{
						found_neighbor_city_in_open_set = true;
						if (current_city.distance + this->GetNeighborDistance(current_city.index, neighbor_city.index) < open_set_top.distance)
						{
							open_set_top.distance = current_city.distance + this->GetNeighborDistance(current_city.index, neighbor_city.index);
							open_set_top.nearest_neighbor_index = current_city.index;
						}
					}
					open_set_temp.push_back(open_set_top);
					open_set.pop();
				}
				for (Neighbor open_set_temp_city : open_set_temp)
					open_set.push(open_set_temp_city);
				//if did not find neighbor city in open set then add it to open set
				if (!found_neighbor_city_in_open_set)
				{
					neighbor_city.nearest_neighbor_index = current_city.index;
					neighbor_city.distance += current_city.distance;
					open_set.push(neighbor_city);
				}
				//PrintOpenSet(open_set);
			}
		}
	}
	//calculate sum of distances of cities to origin city
	cout << "Dijkstras Algorithm Nearest City Paths and Distance to Origin:" << endl;
	for (Neighbor city : closed_set)
	{
		this->avg_dist_ += city.distance / (closed_set.size() - 1);
		if(city.nearest_neighbor_index >= 0)
			printf("( %2d | %0.1f )", city.index, city.distance);
		else
		{
			printf("(  origin  )\n");
			continue;
		}
		//print shortest paths to origin city
		Neighbor nearest_city = city;
		do 
		{
			for (Neighbor next_city : closed_set)
				if (next_city.index == nearest_city.nearest_neighbor_index)
				{
					nearest_city = next_city;
					break;
				}
			cout << "  ->  ";
			if(nearest_city.nearest_neighbor_index >= 0)
				printf("( %2d | %0.1f )", nearest_city.index, nearest_city.distance);
			else
				printf("(  origin  )");
		} while (nearest_city.nearest_neighbor_index >= 0);
		cout << endl;
	}
	cout << "(connected cities " << closed_set.size();
	printf(" | avg path length %0.2f )\n\n\n", this->avg_dist_);
}

void printResultLine(CityGraph& city_graph_object)
{
	printf("*\t%d\t|\t%d\t|\t%.2f\t*\n", city_graph_object.get_size_(), city_graph_object.get_density_(), city_graph_object.avg_dist_);
}

int main() 
{
	cout << "\n**** Avg Path Length Using Dijkstras Algorithm ****" << endl << endl;
	//test dataset
	CityGraph city_graph_object;
	//generate randomly generated city graph objects
	CityGraph city_graph_object1(50, 20);
	CityGraph city_graph_object2(50, 40);
	//print results
	cout << endl << endl;
	for (int i = 0; i < 6; i++) cout << "********";
	cout << "*" << endl;
	cout << "*   Results of Randomly Generated City Graphs\t*" << endl;
	cout << "*      size\t|     density\t|     avg dist\t*" << endl;
	printResultLine(city_graph_object1);
	printResultLine(city_graph_object2);
	for (int i = 0; i < 6; i++) cout << "********";
	cout << "*" << endl << endl;

	return 0;
}


/*
* 
* OUTPUT
* 
* 
C:\Programming\3.C++forCProgrammers\HW2_DijkstraAlgorithm>HW2_DijkstraAlgorithm.exe

**** Avg Path Length Using Dijkstras Algorithm ****

Test Data City Graph with size 9
Implementing graph shown here https ://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
Edges:  |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   1    |    1  |    -  |    1  |    0  |    0  |    0  |    0  |    1  |    0
   2    |    0  |    1  |    -  |    1  |    0  |    1  |    0  |    0  |    1
   3    |    0  |    0  |    1  |    -  |    1  |    1  |    0  |    0  |    0
   4    |    0  |    0  |    0  |    1  |    -  |    1  |    0  |    0  |    0
   5    |    0  |    0  |    1  |    1  |    1  |    -  |    1  |    0  |    0
   6    |    0  |    0  |    0  |    0  |    0  |    1  |    -  |    1  |    1
   7    |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    -  |    1
   8    |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    -

Dist:   |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |  4.0  |    0  |    0  |    0  |    0  |    0  |  8.0  |    0
   1    |  4.0  |    -  |  8.0  |    0  |    0  |    0  |    0  |  11.0 |    0
   2    |    0  |  8.0  |    -  |  7.0  |    0  |  4.0  |    0  |    0  |  2.0
   3    |    0  |    0  |  7.0  |    -  |  9.0  |  14.0 |    0  |    0  |    0
   4    |    0  |    0  |    0  |  9.0  |    -  |  10.0 |    0  |    0  |    0
   5    |    0  |    0  |  4.0  |  14.0 |  10.0 |    -  |  2.0  |    0  |    0
   6    |    0  |    0  |    0  |    0  |    0  |  2.0  |    -  |  1.0  |  6.0
   7    |  8.0  |  11.0 |    0  |    0  |    0  |    0  |  1.0  |    -  |  7.0
   8    |    0  |    0  |  2.0  |    0  |    0  |    0  |  6.0  |  7.0  |    -

Dijkstras Algorithm Nearest City Paths and Distance to Origin:
(  origin  )
(  1 | 4.0 )  ->  (  origin  )
(  7 | 8.0 )  ->  (  origin  )
(  6 | 9.0 )  ->  (  7 | 8.0 )  ->  (  origin  )
(  5 | 11.0 )  ->  (  6 | 9.0 )  ->  (  7 | 8.0 )  ->  (  origin  )
(  2 | 12.0 )  ->  (  1 | 4.0 )  ->  (  origin  )
(  8 | 14.0 )  ->  (  2 | 12.0 )  ->  (  1 | 4.0 )  ->  (  origin  )
(  3 | 19.0 )  ->  (  2 | 12.0 )  ->  (  1 | 4.0 )  ->  (  origin  )
(  4 | 21.0 )  ->  (  5 | 11.0 )  ->  (  6 | 9.0 )  ->  (  7 | 8.0 )  ->  (  origin  )
(connected cities 9 | avg path length 12.25 )
Generated City Graph with size 50 and density 20
Edges:  |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   10  |   11  |   12
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   1    |    0  |    -  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0
   2    |    0  |    0  |    -  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   3    |    1  |    0  |    0  |    -  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   4    |    0  |    0  |    0  |    1  |    -  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   5    |    0  |    0  |    0  |    0  |    1  |    -  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   6    |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    1  |    0  |    0  |    1  |    0  |    0
   7    |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    -  |    0  |    0  |    0  |    0  |    0
   8    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    -  |    1  |    0  |    0  |    0
   9    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    -  |    0  |    0  |    0
   10   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    -  |    0  |    1
   11   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    -  |    0
   12   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    -
   13   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   14   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   15   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   16   |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   17   |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1
   18   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   19   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   20   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0
   21   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   22   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    0  |    0
   23   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0
   24   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   25   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1
   26   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1
   27   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0
   28   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   29   |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1
   30   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    1
   31   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   32   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   33   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   34   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   35   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   36   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   37   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1
   38   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   39   |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   40   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   41   |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   42   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   43   |    1  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1
   44   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   45   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0
   46   |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    1
   47   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   48   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   49   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0

Edges:  |   13  |   14  |   15  |   16  |   17  |   18  |   19  |   20  |   21  |   22  |   23  |   24  |   25
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0
   1    |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   2    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   3    |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   4    |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   5    |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0
   6    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1
   7    |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   8    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1
   9    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0
   10   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0
   11   |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    1
   12   |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1
   13   |    -  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   14   |    0  |    -  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    1
   15   |    0  |    0  |    -  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0
   16   |    0  |    0  |    0  |    -  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   17   |    0  |    0  |    0  |    0  |    -  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0
   18   |    0  |    0  |    0  |    1  |    0  |    -  |    0  |    1  |    1  |    0  |    1  |    0  |    1
   19   |    0  |    0  |    0  |    1  |    0  |    0  |    -  |    0  |    0  |    0  |    0  |    0  |    0
   20   |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    -  |    1  |    0  |    1  |    1  |    0
   21   |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    -  |    0  |    0  |    0  |    1
   22   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    0  |    0  |    0
   23   |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    -  |    0  |    0
   24   |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    -  |    0
   25   |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    -
   26   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   27   |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   28   |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   29   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1
   30   |    1  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   31   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   32   |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   33   |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   34   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1
   35   |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   36   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0
   37   |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0
   38   |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0
   39   |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    1
   40   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   41   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   42   |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1
   43   |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0
   44   |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   45   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   46   |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   47   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1
   48   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   49   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0

Edges:  |   26  |   27  |   28  |   29  |   30  |   31  |   32  |   33  |   34  |   35  |   36  |   37  |   38
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   1    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   2    |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   3    |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   4    |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   5    |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   6    |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   7    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0
   8    |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   9    |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   10   |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0
   11   |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   12   |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1
   13   |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    0  |    1  |    0
   14   |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    1
   15   |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1
   16   |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0
   17   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1
   18   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   19   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   20   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0
   21   |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1
   22   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   23   |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   24   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1
   25   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   26   |    -  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   27   |    0  |    -  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   28   |    1  |    0  |    -  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   29   |    0  |    0  |    0  |    -  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   30   |    0  |    0  |    0  |    0  |    -  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   31   |    0  |    0  |    0  |    0  |    0  |    -  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   32   |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    1  |    0  |    0  |    0  |    1  |    0
   33   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    -  |    0  |    0  |    1  |    0  |    0
   34   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    -  |    0  |    0  |    0  |    0
   35   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    1  |    0  |    0
   36   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    -  |    0  |    0
   37   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    1
   38   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    -
   39   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1
   40   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   41   |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   42   |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   43   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   44   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0
   45   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   46   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   47   |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0
   48   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   49   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0

Edges:  |   39  |   40  |   41  |   42  |   43  |   44  |   45  |   46  |   47  |   48  |   49
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   1    |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0
   2    |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   3    |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   4    |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   5    |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   6    |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0
   7    |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   8    |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0
   9    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   10   |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   11   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1
   12   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0
   13   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   14   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   15   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1
   16   |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0
   17   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   18   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   19   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   20   |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   21   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   22   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   23   |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   24   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   25   |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   26   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0
   27   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   28   |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   29   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   30   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   31   |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   32   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   33   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1
   34   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   35   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0
   36   |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   37   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1
   38   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   39   |    -  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   40   |    1  |    -  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   41   |    1  |    1  |    -  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   42   |    0  |    1  |    0  |    -  |    1  |    1  |    0  |    0  |    0  |    0  |    0
   43   |    0  |    0  |    1  |    1  |    -  |    0  |    0  |    0  |    0  |    0  |    0
   44   |    0  |    0  |    0  |    1  |    0  |    -  |    0  |    0  |    0  |    0  |    0
   45   |    0  |    0  |    0  |    0  |    0  |    0  |    -  |    1  |    0  |    0  |    1
   46   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    -  |    1  |    0  |    1
   47   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    -  |    0  |    0
   48   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    -  |    0
   49   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    -

Dist:   |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   10  |   11  |   12
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |    0  |    0  |  2.5  |  1.0  |    0  |  5.2  |  2.1  |  8.6  |    0  |    0  |    0  |  6.4
   1    |    0  |    -  |    0  |    0  |  8.7  |    0  |  7.2  |  5.2  |    0  |    0  |    0  |  1.0  |  4.4
   2    |    0  |    0  |    -  |    0  |    0  |    0  |    0  |  6.8  |    0  |    0  |    0  |    0  |    0
   3    |  2.5  |    0  |    0  |    -  |  3.6  |    0  |    0  |    0  |    0  |  7.1  |    0  |    0  |    0
   4    |  1.0  |  8.7  |    0  |  3.6  |    -  |  1.9  |    0  |  3.2  |    0  |  7.7  |    0  |    0  |    0
   5    |    0  |    0  |    0  |    0  |  1.9  |    -  |  7.2  |    0  |    0  |  9.3  |  5.2  |  1.2  |    0
   6    |  5.2  |  7.2  |    0  |    0  |    0  |  7.2  |    -  |  4.8  |  2.1  |    0  |  6.7  |    0  |    0
   7    |  2.1  |  5.2  |  6.8  |    0  |  3.2  |    0  |  4.8  |    -  |    0  |    0  |    0  |    0  |    0
   8    |  8.6  |    0  |    0  |    0  |    0  |    0  |  2.1  |    0  |    -  |  3.8  |    0  |    0  |    0
   9    |    0  |    0  |    0  |  7.1  |  7.7  |  9.3  |    0  |    0  |  3.8  |    -  |    0  |    0  |    0
   10   |    0  |    0  |    0  |    0  |    0  |  5.2  |  6.7  |    0  |    0  |    0  |    -  |    0  |  2.0
   11   |    0  |  1.0  |    0  |    0  |    0  |  1.2  |    0  |    0  |    0  |    0  |    0  |    -  |  9.4
   12   |  6.4  |  4.4  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  2.0  |  9.4  |    -
   13   |    0  |  2.4  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  7.1
   14   |    0  |    0  |    0  |  4.0  |  1.4  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   15   |    0  |    0  |    0  |    0  |    0  |    0  |  9.7  |    0  |    0  |    0  |  3.5  |    0  |  5.9
   16   |  5.4  |  6.6  |    0  |    0  |  5.4  |    0  |    0  |  2.6  |    0  |    0  |    0  |    0  |    0
   17   |  4.4  |  7.5  |    0  |  8.5  |    0  |    0  |    0  |    0  |    0  |    0  |  7.6  |  8.0  |  2.9
   18   |    0  |    0  |  8.2  |  1.6  |    0  |    0  |  8.7  |    0  |  9.8  |    0  |    0  |  6.3  |    0
   19   |    0  |    0  |    0  |    0  |    0  |  6.1  |    0  |  7.8  |    0  |    0  |  5.8  |    0  |  6.6
   20   |  9.5  |    0  |    0  |    0  |  8.4  |  1.3  |    0  |    0  |    0  |  6.0  |    0  |  5.3  |    0
   21   |    0  |    0  |  8.1  |  6.6  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  7.1
   22   |    0  |  8.2  |  3.0  |    0  |    0  |  9.8  |  2.8  |  7.6  |  5.9  |  5.0  |  1.4  |    0  |  2.3
   23   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  2.3  |  3.5  |  5.0  |  5.6  |    0  |    0
   24   |  1.0  |    0  |  5.9  |    0  |  1.1  |    0  |    0  |    0  |    0  |    0  |  4.1  |    0  |  3.1
   25   |    0  |    0  |  2.1  |    0  |    0  |    0  |  7.0  |    0  |  9.2  |    0  |  2.4  |  8.7  |  4.8
   26   |    0  |    0  |    0  |  4.9  |  3.8  |  5.7  |  1.0  |    0  |    0  |    0  |    0  |  1.6  |  2.1
   27   |  4.0  |  6.0  |  4.9  |  5.8  |    0  |  6.0  |  4.1  |    0  |  6.5  |    0  |    0  |    0  |  2.1
   28   |    0  |  5.3  |    0  |    0  |    0  |  7.6  |    0  |    0  |    0  |    0  |  4.1  |    0  |    0
   29   |    0  |  9.1  |  8.3  |  4.3  |  4.7  |    0  |  6.7  |    0  |  9.4  |    0  |  1.0  |    0  |  3.5
   30   |    0  |    0  |    0  |    0  |  5.4  |    0  |    0  |    0  |    0  |  5.3  |  8.8  |  7.5  |  3.7
   31   |  7.9  |    0  |    0  |  9.5  |  4.6  |    0  |  4.8  |  1.2  |  4.1  |  8.3  |    0  |  3.4  |    0
   32   |  8.4  |    0  |    0  |  4.6  |    0  |    0  |    0  |    0  |    0  |    0  |  5.5  |    0  |  9.2
   33   |  2.6  |    0  |    0  |  4.0  |    0  |  5.3  |    0  |    0  |    0  |    0  |  7.1  |    0  |    0
   34   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  3.2  |    0  |  7.8  |    0  |    0  |    0
   35   |    0  |    0  |  8.9  |    0  |    0  |  1.9  |    0  |    0  |    0  |    0  |    0  |  9.6  |  6.8
   36   |    0  |    0  |  3.8  |    0  |    0  |    0  |    0  |  6.7  |    0  |    0  |  6.9  |    0  |    0
   37   |  1.7  |    0  |    0  |  1.7  |    0  |    0  |  1.0  |  5.7  |    0  |    0  |    0  |    0  |  2.7
   38   |    0  |    0  |    0  |    0  |  9.2  |    0  |  8.2  |    0  |    0  |    0  |    0  |    0  |  9.5
   39   |    0  |    0  |  3.8  |    0  |    0  |  4.1  |    0  |    0  |  8.8  |    0  |    0  |    0  |    0
   40   |    0  |  5.4  |  3.9  |    0  |    0  |  7.6  |    0  |  7.1  |    0  |    0  |    0  |    0  |    0
   41   |    0  |    0  |  7.2  |    0  |  8.7  |  2.4  |    0  |    0  |    0  |  9.5  |  8.6  |    0  |    0
   42   |  9.6  |    0  |    0  |  3.7  |    0  |    0  |  4.1  |  2.3  |    0  |    0  |    0  |    0  |  1.1
   43   |  2.6  |  7.8  |  9.2  |    0  |    0  |    0  |  3.8  |  4.6  |  3.7  |    0  |    0  |    0  |  7.6
   44   |    0  |    0  |    0  |    0  |    0  |  6.9  |    0  |  2.7  |    0  |    0  |  1.0  |  4.3  |    0
   45   |    0  |  8.2  |    0  |    0  |  1.0  |    0  |    0  |    0  |  1.9  |    0  |  7.4  |  4.3  |    0
   46   |    0  |  6.5  |    0  |    0  |    0  |    0  |  8.5  |  4.3  |  8.1  |  8.7  |  9.3  |    0  |  6.7
   47   |    0  |    0  |    0  |  8.0  |    0  |    0  |    0  |  9.5  |  5.3  |    0  |    0  |    0  |    0
   48   |  6.2  |  4.5  |    0  |    0  |    0  |  4.7  |  4.5  |  3.1  |    0  |    0  |    0  |    0  |    0
   49   |  3.8  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  8.7  |    0

Dist:   |   13  |   14  |   15  |   16  |   17  |   18  |   19  |   20  |   21  |   22  |   23  |   24  |   25
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |  5.4  |  4.4  |    0  |    0  |  9.5  |    0  |    0  |    0  |  1.0  |    0
   1    |  2.4  |    0  |    0  |  6.6  |  7.5  |    0  |    0  |    0  |    0  |  8.2  |    0  |    0  |    0
   2    |    0  |    0  |    0  |    0  |    0  |  8.2  |    0  |    0  |  8.1  |  3.0  |    0  |  5.9  |  2.1
   3    |    0  |  4.0  |    0  |    0  |  8.5  |  1.6  |    0  |    0  |  6.6  |    0  |    0  |    0  |    0
   4    |    0  |  1.4  |    0  |  5.4  |    0  |    0  |    0  |  8.4  |    0  |    0  |    0  |  1.1  |    0
   5    |    0  |    0  |    0  |    0  |    0  |    0  |  6.1  |  1.3  |    0  |  9.8  |    0  |    0  |    0
   6    |    0  |    0  |  9.7  |    0  |    0  |  8.7  |    0  |    0  |    0  |  2.8  |    0  |    0  |  7.0
   7    |    0  |    0  |    0  |  2.6  |    0  |    0  |  7.8  |    0  |    0  |  7.6  |  2.3  |    0  |    0
   8    |    0  |    0  |    0  |    0  |    0  |  9.8  |    0  |    0  |    0  |  5.9  |  3.5  |    0  |  9.2
   9    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  6.0  |    0  |  5.0  |  5.0  |    0  |    0
   10   |    0  |    0  |  3.5  |    0  |  7.6  |    0  |  5.8  |    0  |    0  |  1.4  |  5.6  |  4.1  |  2.4
   11   |    0  |    0  |    0  |    0  |  8.0  |  6.3  |    0  |  5.3  |    0  |    0  |    0  |    0  |  8.7
   12   |  7.1  |    0  |  5.9  |    0  |  2.9  |    0  |  6.6  |    0  |  7.1  |  2.3  |    0  |  3.1  |  4.8
   13   |    -  |    0  |    0  |    0  |    0  |    0  |    0  |  5.5  |    0  |    0  |    0  |    0  |  3.9
   14   |    0  |    -  |    0  |    0  |    0  |  7.0  |    0  |    0  |    0  |  1.2  |  7.6  |  5.3  |  4.4
   15   |    0  |    0  |    -  |    0  |  3.3  |  3.1  |    0  |  4.0  |    0  |    0  |    0  |  8.2  |    0
   16   |    0  |    0  |    0  |    -  |    0  |  3.9  |  7.6  |  9.0  |    0  |    0  |    0  |    0  |    0
   17   |    0  |    0  |  3.3  |    0  |    -  |    0  |    0  |  4.7  |  8.1  |  9.9  |  6.3  |  6.5  |    0
   18   |    0  |  7.0  |  3.1  |  3.9  |    0  |    -  |    0  |  9.9  |  3.3  |    0  |  5.5  |    0  |  7.5
   19   |    0  |    0  |    0  |  7.6  |    0  |    0  |    -  |    0  |    0  |    0  |  1.0  |  8.9  |    0
   20   |  5.5  |    0  |  4.0  |  9.0  |  4.7  |  9.9  |    0  |    -  |  5.8  |  7.7  |  9.4  |  2.3  |  2.3
   21   |    0  |    0  |    0  |    0  |  8.1  |  3.3  |    0  |  5.8  |    -  |  5.7  |    0  |    0  |  3.8
   22   |    0  |  1.2  |    0  |    0  |  9.9  |    0  |    0  |  7.7  |  5.7  |    -  |    0  |    0  |    0
   23   |    0  |  7.6  |    0  |    0  |  6.3  |  5.5  |  1.0  |  9.4  |    0  |    0  |    -  |    0  |    0
   24   |    0  |  5.3  |  8.2  |    0  |  6.5  |    0  |  8.9  |  2.3  |    0  |    0  |    0  |    -  |    0
   25   |  3.9  |  4.4  |    0  |    0  |    0  |  7.5  |    0  |  2.3  |  3.8  |    0  |    0  |    0  |    -
   26   |    0  |  5.5  |  2.2  |    0  |  3.5  |  3.6  |    0  |    0  |  7.3  |    0  |    0  |    0  |  2.0
   27   |  5.2  |  5.2  |    0  |  1.4  |  1.0  |    0  |  2.5  |    0  |    0  |    0  |  4.1  |    0  |    0
   28   |  4.8  |    0  |  3.3  |    0  |  7.5  |  2.7  |    0  |    0  |  7.5  |    0  |    0  |    0  |    0
   29   |    0  |    0  |  5.1  |  8.4  |  3.8  |    0  |    0  |  6.0  |    0  |    0  |  9.7  |  7.2  |  2.5
   30   |  5.7  |  7.3  |  9.0  |  8.4  |    0  |    0  |    0  |  5.1  |    0  |    0  |  7.9  |    0  |  4.9
   31   |    0  |  2.7  |  3.1  |  7.4  |    0  |    0  |    0  |    0  |    0  |    0  |  4.0  |    0  |    0
   32   |  6.3  |  6.6  |    0  |    0  |  5.9  |    0  |  5.5  |  5.8  |    0  |  4.5  |  4.2  |    0  |    0
   33   |  6.0  |  5.1  |  8.7  |    0  |    0  |    0  |    0  |    0  |  1.8  |    0  |  7.5  |  2.9  |    0
   34   |  5.1  |    0  |    0  |    0  |    0  |    0  |    0  |  6.0  |  2.1  |    0  |  9.6  |  7.8  |  7.4
   35   |  3.1  |    0  |    0  |  3.5  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  3.3  |    0
   36   |  5.4  |    0  |    0  |  3.0  |    0  |    0  |  7.2  |  7.5  |    0  |    0  |    0  |  4.1  |    0
   37   |  6.9  |    0  |    0  |  5.2  |    0  |    0  |    0  |  6.5  |  9.3  |  8.9  |  4.1  |    0  |    0
   38   |  4.0  |  5.5  |  8.4  |  1.3  |  2.2  |  2.1  |    0  |  8.8  |  9.4  |    0  |    0  |  3.2  |    0
   39   |    0  |  5.5  |    0  |  2.8  |  1.3  |    0  |  7.8  |  9.2  |  9.0  |    0  |    0  |    0  |  9.7
   40   |    0  |    0  |    0  |  5.5  |  6.6  |    0  |    0  |    0  |  3.2  |  3.2  |    0  |    0  |    0
   41   |    0  |  6.1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  1.2  |  5.6  |    0  |    0
   42   |  5.8  |    0  |  3.3  |  6.2  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  6.6  |  8.1
   43   |    0  |    0  |  1.5  |  1.6  |    0  |    0  |    0  |  5.7  |    0  |  7.4  |    0  |  3.5  |    0
   44   |  4.3  |  9.0  |    0  |  5.7  |  5.5  |  8.8  |  5.1  |    0  |    0  |    0  |    0  |    0  |    0
   45   |    0  |    0  |  3.7  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  9.2  |    0  |    0
   46   |    0  |  8.5  |  1.7  |  8.6  |    0  |    0  |  4.7  |    0  |    0  |    0  |    0  |    0  |  4.9
   47   |    0  |    0  |    0  |  7.4  |    0  |    0  |    0  |    0  |  5.7  |    0  |    0  |    0  |  2.7
   48   |  2.0  |    0  |  2.4  |    0  |    0  |    0  |    0  |    0  |    0  |  4.4  |  1.0  |    0  |  4.4
   49   |    0  |    0  |  7.2  |    0  |    0  |    0  |    0  |  3.3  |    0  |    0  |    0  |    0  |    0

Dist:   |   26  |   27  |   28  |   29  |   30  |   31  |   32  |   33  |   34  |   35  |   36  |   37  |   38
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |  4.0  |    0  |    0  |    0  |  7.9  |  8.4  |  2.6  |    0  |    0  |    0  |  1.7  |    0
   1    |    0  |  6.0  |  5.3  |  9.1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   2    |    0  |  4.9  |    0  |  8.3  |    0  |    0  |    0  |    0  |    0  |  8.9  |  3.8  |    0  |    0
   3    |  4.9  |  5.8  |    0  |  4.3  |    0  |  9.5  |  4.6  |  4.0  |    0  |    0  |    0  |  1.7  |    0
   4    |  3.8  |    0  |    0  |  4.7  |  5.4  |  4.6  |    0  |    0  |    0  |    0  |    0  |    0  |  9.2
   5    |  5.7  |  6.0  |  7.6  |    0  |    0  |    0  |    0  |  5.3  |    0  |  1.9  |    0  |    0  |    0
   6    |  1.0  |  4.1  |    0  |  6.7  |    0  |  4.8  |    0  |    0  |    0  |    0  |    0  |  1.0  |  8.2
   7    |    0  |    0  |    0  |    0  |    0  |  1.2  |    0  |    0  |  3.2  |    0  |  6.7  |  5.7  |    0
   8    |    0  |  6.5  |    0  |  9.4  |    0  |  4.1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   9    |    0  |    0  |    0  |    0  |  5.3  |  8.3  |    0  |    0  |  7.8  |    0  |    0  |    0  |    0
   10   |    0  |    0  |  4.1  |  1.0  |  8.8  |    0  |  5.5  |  7.1  |    0  |    0  |  6.9  |    0  |    0
   11   |  1.6  |    0  |    0  |    0  |  7.5  |  3.4  |    0  |    0  |    0  |  9.6  |    0  |    0  |    0
   12   |  2.1  |  2.1  |    0  |  3.5  |  3.7  |    0  |  9.2  |    0  |    0  |  6.8  |    0  |  2.7  |  9.5
   13   |    0  |  5.2  |  4.8  |    0  |  5.7  |    0  |  6.3  |  6.0  |  5.1  |  3.1  |  5.4  |  6.9  |  4.0
   14   |  5.5  |  5.2  |    0  |    0  |  7.3  |  2.7  |  6.6  |  5.1  |    0  |    0  |    0  |    0  |  5.5
   15   |  2.2  |    0  |  3.3  |  5.1  |  9.0  |  3.1  |    0  |  8.7  |    0  |    0  |    0  |    0  |  8.4
   16   |    0  |  1.4  |    0  |  8.4  |  8.4  |  7.4  |    0  |    0  |    0  |  3.5  |  3.0  |  5.2  |  1.3
   17   |  3.5  |  1.0  |  7.5  |  3.8  |    0  |    0  |  5.9  |    0  |    0  |    0  |    0  |    0  |  2.2
   18   |  3.6  |    0  |  2.7  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  2.1
   19   |    0  |  2.5  |    0  |    0  |    0  |    0  |  5.5  |    0  |    0  |    0  |  7.2  |    0  |    0
   20   |    0  |    0  |    0  |  6.0  |  5.1  |    0  |  5.8  |    0  |  6.0  |    0  |  7.5  |  6.5  |  8.8
   21   |  7.3  |    0  |  7.5  |    0  |    0  |    0  |    0  |  1.8  |  2.1  |    0  |    0  |  9.3  |  9.4
   22   |    0  |    0  |    0  |    0  |    0  |    0  |  4.5  |    0  |    0  |    0  |    0  |  8.9  |    0
   23   |    0  |  4.1  |    0  |  9.7  |  7.9  |  4.0  |  4.2  |  7.5  |  9.6  |    0  |    0  |  4.1  |    0
   24   |    0  |    0  |    0  |  7.2  |    0  |    0  |    0  |  2.9  |  7.8  |  3.3  |  4.1  |    0  |  3.2
   25   |  2.0  |    0  |    0  |  2.5  |  4.9  |    0  |    0  |    0  |  7.4  |    0  |    0  |    0  |    0
   26   |    -  |    0  |  6.9  |    0  |  8.0  |    0  |    0  |  3.0  |    0  |  2.8  |  3.6  |    0  |    0
   27   |    0  |    -  |    0  |    0  |  4.0  |    0  |  9.7  |    0  |  2.5  |    0  |    0  |    0  |    0
   28   |  6.9  |    0  |    -  |    0  |    0  |    0  |    0  |    0  |  3.9  |    0  |    0  |  3.3  |    0
   29   |    0  |    0  |    0  |    -  |  2.1  |    0  |    0  |    0  |  8.7  |  2.6  |    0  |    0  |  7.8
   30   |  8.0  |  4.0  |    0  |  2.1  |    -  |  5.2  |    0  |    0  |    0  |  7.4  |  8.7  |  2.5  |    0
   31   |    0  |    0  |    0  |    0  |  5.2  |    -  |  4.7  |  2.0  |    0  |    0  |  6.5  |    0  |    0
   32   |    0  |  9.7  |    0  |    0  |    0  |  4.7  |    -  |  2.5  |    0  |  7.2  |  8.6  |  7.6  |    0
   33   |  3.0  |    0  |    0  |    0  |    0  |  2.0  |  2.5  |    -  |    0  |  7.0  |  5.5  |  8.3  |  9.0
   34   |    0  |  2.5  |  3.9  |  8.7  |    0  |    0  |    0  |    0  |    -  |    0  |    0  |  5.1  |    0
   35   |  2.8  |    0  |    0  |  2.6  |  7.4  |    0  |  7.2  |  7.0  |    0  |    -  |  5.0  |    0  |  6.6
   36   |  3.6  |    0  |    0  |    0  |  8.7  |  6.5  |  8.6  |  5.5  |    0  |  5.0  |    -  |    0  |    0
   37   |    0  |    0  |  3.3  |    0  |  2.5  |    0  |  7.6  |  8.3  |  5.1  |    0  |    0  |    -  |  2.3
   38   |    0  |    0  |    0  |  7.8  |    0  |    0  |    0  |  9.0  |    0  |  6.6  |    0  |  2.3  |    -
   39   |    0  |  3.1  |  2.8  |    0  |    0  |  7.3  |  5.0  |    0  |  1.8  |    0  |  2.4  |  8.0  |  7.6
   40   |    0  |    0  |  8.7  |    0  |    0  |  2.2  |    0  |    0  |    0  |  1.6  |  9.0  |    0  |    0
   41   |    0  |  4.8  |  6.0  |    0  |  8.7  |  3.1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   42   |  2.9  |    0  |    0  |  5.6  |    0  |    0  |    0  |    0  |    0  |  3.9  |    0  |    0  |    0
   43   |  9.9  |    0  |    0  |    0  |  2.0  |  8.2  |    0  |    0  |  8.0  |    0  |    0  |    0  |  6.6
   44   |    0  |    0  |  3.5  |    0  |  8.2  |    0  |    0  |    0  |    0  |  7.5  |    0  |  9.2  |    0
   45   |    0  |    0  |    0  |    0  |  1.0  |    0  |    0  |  2.8  |    0  |  9.2  |    0  |    0  |    0
   46   |    0  |    0  |  1.3  |    0  |  4.4  |    0  |  5.0  |    0  |    0  |    0  |    0  |  9.6  |  1.1
   47   |  9.0  |    0  |  1.3  |    0  |    0  |    0  |  6.3  |    0  |  4.6  |  3.2  |  3.8  |    0  |    0
   48   |  9.6  |    0  |  8.5  |    0  |    0  |    0  |    0  |    0  |  7.4  |    0  |    0  |    0  |    0
   49   |    0  |    0  |  3.8  |    0  |    0  |  1.0  |  8.5  |  2.0  |  4.0  |    0  |  1.4  |  3.0  |    0

Dist:   |   39  |   40  |   41  |   42  |   43  |   44  |   45  |   46  |   47  |   48  |   49
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |  9.6  |  2.6  |    0  |    0  |    0  |    0  |  6.2  |  3.8
   1    |    0  |  5.4  |    0  |    0  |  7.8  |    0  |  8.2  |  6.5  |    0  |  4.5  |    0
   2    |  3.8  |  3.9  |  7.2  |    0  |  9.2  |    0  |    0  |    0  |    0  |    0  |    0
   3    |    0  |    0  |    0  |  3.7  |    0  |    0  |    0  |    0  |  8.0  |    0  |    0
   4    |    0  |    0  |  8.7  |    0  |    0  |    0  |  1.0  |    0  |    0  |    0  |    0
   5    |  4.1  |  7.6  |  2.4  |    0  |    0  |  6.9  |    0  |    0  |    0  |  4.7  |    0
   6    |    0  |    0  |    0  |  4.1  |  3.8  |    0  |    0  |  8.5  |    0  |  4.5  |    0
   7    |    0  |  7.1  |    0  |  2.3  |  4.6  |  2.7  |    0  |  4.3  |  9.5  |  3.1  |    0
   8    |  8.8  |    0  |    0  |    0  |  3.7  |    0  |  1.9  |  8.1  |  5.3  |    0  |    0
   9    |    0  |    0  |  9.5  |    0  |    0  |    0  |    0  |  8.7  |    0  |    0  |    0
   10   |    0  |    0  |  8.6  |    0  |    0  |  1.0  |  7.4  |  9.3  |    0  |    0  |    0
   11   |    0  |    0  |    0  |    0  |    0  |  4.3  |  4.3  |    0  |    0  |    0  |  8.7
   12   |    0  |    0  |    0  |  1.1  |  7.6  |    0  |    0  |  6.7  |    0  |    0  |    0
   13   |    0  |    0  |    0  |  5.8  |    0  |  4.3  |    0  |    0  |    0  |  2.0  |    0
   14   |  5.5  |    0  |  6.1  |    0  |    0  |  9.0  |    0  |  8.5  |    0  |    0  |    0
   15   |    0  |    0  |    0  |  3.3  |  1.5  |    0  |  3.7  |  1.7  |    0  |  2.4  |  7.2
   16   |  2.8  |  5.5  |    0  |  6.2  |  1.6  |  5.7  |    0  |  8.6  |  7.4  |    0  |    0
   17   |  1.3  |  6.6  |    0  |    0  |    0  |  5.5  |    0  |    0  |    0  |    0  |    0
   18   |    0  |    0  |    0  |    0  |    0  |  8.8  |    0  |    0  |    0  |    0  |    0
   19   |  7.8  |    0  |    0  |    0  |    0  |  5.1  |    0  |  4.7  |    0  |    0  |    0
   20   |  9.2  |    0  |    0  |    0  |  5.7  |    0  |    0  |    0  |    0  |    0  |  3.3
   21   |  9.0  |  3.2  |    0  |    0  |    0  |    0  |    0  |    0  |  5.7  |    0  |    0
   22   |    0  |  3.2  |  1.2  |    0  |  7.4  |    0  |    0  |    0  |    0  |  4.4  |    0
   23   |    0  |    0  |  5.6  |    0  |    0  |    0  |  9.2  |    0  |    0  |  1.0  |    0
   24   |    0  |    0  |    0  |  6.6  |  3.5  |    0  |    0  |    0  |    0  |    0  |    0
   25   |  9.7  |    0  |    0  |  8.1  |    0  |    0  |    0  |  4.9  |  2.7  |  4.4  |    0
   26   |    0  |    0  |    0  |  2.9  |  9.9  |    0  |    0  |    0  |  9.0  |  9.6  |    0
   27   |  3.1  |    0  |  4.8  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   28   |  2.8  |  8.7  |  6.0  |    0  |    0  |  3.5  |    0  |  1.3  |  1.3  |  8.5  |  3.8
   29   |    0  |    0  |    0  |  5.6  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   30   |    0  |    0  |  8.7  |    0  |  2.0  |  8.2  |  1.0  |  4.4  |    0  |    0  |    0
   31   |  7.3  |  2.2  |  3.1  |    0  |  8.2  |    0  |    0  |    0  |    0  |    0  |  1.0
   32   |  5.0  |    0  |    0  |    0  |    0  |    0  |    0  |  5.0  |  6.3  |    0  |  8.5
   33   |    0  |    0  |    0  |    0  |    0  |    0  |  2.8  |    0  |    0  |    0  |  2.0
   34   |  1.8  |    0  |    0  |    0  |  8.0  |    0  |    0  |    0  |  4.6  |  7.4  |  4.0
   35   |    0  |  1.6  |    0  |  3.9  |    0  |  7.5  |  9.2  |    0  |  3.2  |    0  |    0
   36   |  2.4  |  9.0  |    0  |    0  |    0  |    0  |    0  |    0  |  3.8  |    0  |  1.4
   37   |  8.0  |    0  |    0  |    0  |    0  |  9.2  |    0  |  9.6  |    0  |    0  |  3.0
   38   |  7.6  |    0  |    0  |    0  |  6.6  |    0  |    0  |  1.1  |    0  |    0  |    0
   39   |    -  |  5.6  |  2.6  |  8.7  |  2.4  |    0  |    0  |    0  |    0  |    0  |    0
   40   |  5.6  |    -  |  6.9  |  9.7  |    0  |  3.0  |    0  |    0  |    0  |  6.0  |  6.2
   41   |  2.6  |  6.9  |    -  |  9.9  |  1.4  |    0  |    0  |    0  |    0  |  7.5  |  5.5
   42   |  8.7  |  9.7  |  9.9  |    -  |  4.6  |  3.8  |    0  |    0  |    0  |    0  |    0
   43   |  2.4  |    0  |  1.4  |  4.6  |    -  |    0  |    0  |  8.5  |    0  |  9.6  |    0
   44   |    0  |  3.0  |    0  |  3.8  |    0  |    -  |    0  |    0  |    0  |  2.1  |    0
   45   |    0  |    0  |    0  |    0  |    0  |    0  |    -  |  9.1  |  5.4  |    0  |  4.1
   46   |    0  |    0  |    0  |    0  |  8.5  |    0  |  9.1  |    -  |  4.0  |    0  |  8.1
   47   |    0  |    0  |    0  |    0  |    0  |    0  |  5.4  |  4.0  |    -  |    0  |    0
   48   |    0  |  6.0  |  7.5  |    0  |  9.6  |  2.1  |    0  |    0  |    0  |    -  |    0
   49   |    0  |  6.2  |  5.5  |    0  |    0  |    0  |  4.1  |  8.1  |    0  |    0  |    -

Dijkstras Algorithm Nearest City Paths and Distance to Origin:
(  origin  )
( 24 | 1.0 )  ->  (  origin  )
(  3 | 2.5 )  ->  (  origin  )
( 43 | 2.6 )  ->  (  origin  )
( 33 | 2.6 )  ->  (  origin  )
( 20 | 3.3 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 41 | 4.0 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 15 | 4.1 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 18 | 4.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 38 | 4.2 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 17 | 4.4 )  ->  (  origin  )
( 49 | 4.6 )  ->  ( 33 | 2.6 )  ->  (  origin  )
( 31 | 4.6 )  ->  ( 33 | 2.6 )  ->  (  origin  )
(  5 | 4.6 )  ->  ( 20 | 3.3 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 36 | 5.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 10 | 5.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 32 | 5.1 )  ->  ( 33 | 2.6 )  ->  (  origin  )
( 45 | 5.4 )  ->  ( 33 | 2.6 )  ->  (  origin  )
( 27 | 5.6 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 39 | 5.7 )  ->  ( 17 | 4.4 )  ->  (  origin  )
(  4 | 6.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 29 | 6.1 )  ->  ( 10 | 5.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 44 | 6.1 )  ->  ( 10 | 5.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 42 | 6.2 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 26 | 6.2 )  ->  ( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 14 | 6.3 )  ->  ( 24 | 1.0 )  ->  (  origin  )
(  6 | 6.4 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 30 | 6.4 )  ->  ( 45 | 5.4 )  ->  ( 33 | 2.6 )  ->  (  origin  )
( 37 | 6.5 )  ->  ( 38 | 4.2 )  ->  ( 24 | 1.0 )  ->  (  origin  )
(  7 | 6.8 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 28 | 6.8 )  ->  ( 18 | 4.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
(  8 | 7.3 )  ->  ( 45 | 5.4 )  ->  ( 33 | 2.6 )  ->  (  origin  )
( 21 | 7.4 )  ->  ( 18 | 4.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 34 | 7.5 )  ->  ( 39 | 5.7 )  ->  ( 17 | 4.4 )  ->  (  origin  )
( 35 | 7.7 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 11 | 7.8 )  ->  ( 26 | 6.2 )  ->  ( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 25 | 8.6 )  ->  ( 29 | 6.1 )  ->  ( 10 | 5.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
(  1 | 8.8 )  ->  ( 11 | 7.8 )  ->  ( 26 | 6.2 )  ->  ( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 22 | 9.2 )  ->  (  6 | 6.4 )  ->  ( 43 | 2.6 )  ->  (  origin  )
(  9 | 9.3 )  ->  ( 20 | 3.3 )  ->  ( 24 | 1.0 )  ->  (  origin  )
(  2 | 9.5 )  ->  ( 39 | 5.7 )  ->  ( 17 | 4.4 )  ->  (  origin  )
( 23 | 9.6 )  ->  ( 18 | 4.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 40 | 9.7 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 48 | 10.6 )  ->  ( 23 | 9.6 )  ->  ( 18 | 4.1 )  ->  (  3 | 2.5 )  ->  (  origin  )
( 19 | 10.7 )  ->  ( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 46 | 10.8 )  ->  ( 12 | 4.1 )  ->  ( 24 | 1.0 )  ->  (  origin  )
( 13 | 10.8 )  ->  ( 35 | 7.7 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
( 47 | 10.9 )  ->  ( 35 | 7.7 )  ->  ( 16 | 4.2 )  ->  ( 43 | 2.6 )  ->  (  origin  )
(connected cities 50 | avg path length 6.39 )
Generated City Graph with size 50 and density 40
Edges:  |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   10  |   11  |   12
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    1  |    0
   1    |    0  |    -  |    1  |    1  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    1  |    0
   2    |    0  |    1  |    -  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1
   3    |    0  |    1  |    1  |    -  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0
   4    |    0  |    0  |    1  |    0  |    -  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   5    |    0  |    1  |    0  |    0  |    1  |    -  |    1  |    1  |    1  |    0  |    1  |    0  |    1
   6    |    1  |    1  |    0  |    1  |    0  |    1  |    -  |    0  |    1  |    1  |    0  |    0  |    1
   7    |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    -  |    0  |    0  |    0  |    1  |    1
   8    |    1  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    -  |    1  |    0  |    1  |    0
   9    |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    -  |    0  |    0  |    0
   10   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    -  |    1  |    1
   11   |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    -  |    0
   12   |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    -
   13   |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1
   14   |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1
   15   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1
   16   |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1
   17   |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0
   18   |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    1  |    0
   19   |    0  |    1  |    1  |    1  |    1  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   20   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0
   21   |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1
   22   |    0  |    1  |    1  |    1  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    1
   23   |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    1
   24   |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    0
   25   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1
   26   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   27   |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   28   |    1  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   29   |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0
   30   |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0
   31   |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1
   32   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   33   |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1
   34   |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    0
   35   |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0
   36   |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0
   37   |    1  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0
   38   |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   39   |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0
   40   |    1  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   41   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1
   42   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   43   |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0
   44   |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    0
   45   |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1
   46   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0
   47   |    1  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   48   |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0
   49   |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0

Edges:  |   13  |   14  |   15  |   16  |   17  |   18  |   19  |   20  |   21  |   22  |   23  |   24  |   25
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   1    |    1  |    1  |    0  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    0  |    0
   2    |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0
   3    |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    1  |    1  |    0  |    1  |    0
   4    |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    1  |    0
   5    |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    1  |    1
   6    |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    0  |    0
   7    |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   8    |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    0
   9    |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   10   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    0
   11   |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   12   |    1  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    1
   13   |    -  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1
   14   |    0  |    -  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    0
   15   |    1  |    0  |    -  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    1  |    0
   16   |    0  |    1  |    0  |    -  |    1  |    0  |    1  |    1  |    1  |    1  |    0  |    1  |    0
   17   |    0  |    0  |    0  |    1  |    -  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    0
   18   |    0  |    1  |    1  |    0  |    1  |    -  |    0  |    1  |    0  |    1  |    0  |    1  |    0
   19   |    0  |    1  |    1  |    1  |    0  |    0  |    -  |    1  |    0  |    0  |    0  |    1  |    0
   20   |    1  |    0  |    1  |    1  |    0  |    1  |    1  |    -  |    1  |    0  |    0  |    0  |    1
   21   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    -  |    0  |    1  |    1  |    0
   22   |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    -  |    0  |    1  |    1
   23   |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    -  |    0  |    0
   24   |    0  |    1  |    1  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    -  |    1
   25   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    -
   26   |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    1
   27   |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   28   |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1
   29   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1
   30   |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    1  |    0
   31   |    1  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   32   |    1  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    0
   33   |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1
   34   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1
   35   |    1  |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    1
   36   |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   37   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   38   |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    1
   39   |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    1  |    0
   40   |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0
   41   |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1
   42   |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    1
   43   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    1
   44   |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    1
   45   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   46   |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0
   47   |    0  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    0  |    1
   48   |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   49   |    1  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    1  |    1

Edges:  |   26  |   27  |   28  |   29  |   30  |   31  |   32  |   33  |   34  |   35  |   36  |   37  |   38
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    1  |    1  |    0
   1    |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1
   2    |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0
   3    |    1  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    1
   4    |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   5    |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0
   6    |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0
   7    |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0
   8    |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0
   9    |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0
   10   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   11   |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    1  |    1
   12   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   13   |    0  |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    0
   14   |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    1
   15   |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    0
   16   |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    1
   17   |    0  |    1  |    1  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    1
   18   |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    1
   19   |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0
   20   |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   21   |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    0
   22   |    1  |    1  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1
   23   |    1  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    1
   24   |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0
   25   |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    1
   26   |    -  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    1  |    1  |    0  |    0
   27   |    0  |    -  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    1
   28   |    0  |    1  |    -  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    1  |    1  |    1
   29   |    1  |    0  |    0  |    -  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   30   |    1  |    1  |    1  |    0  |    -  |    1  |    1  |    1  |    1  |    1  |    1  |    0  |    0
   31   |    0  |    0  |    0  |    1  |    1  |    -  |    1  |    0  |    0  |    0  |    0  |    1  |    0
   32   |    1  |    0  |    1  |    0  |    1  |    1  |    -  |    0  |    0  |    1  |    1  |    1  |    1
   33   |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    -  |    1  |    1  |    1  |    1  |    0
   34   |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    -  |    1  |    0  |    1  |    0
   35   |    1  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    1  |    -  |    1  |    0  |    1
   36   |    1  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    1  |    -  |    0  |    0
   37   |    0  |    1  |    1  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    -  |    0
   38   |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    -
   39   |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    1
   40   |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    0  |    0
   41   |    1  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0
   42   |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    0  |    0  |    1
   43   |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    1
   44   |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1  |    0
   45   |    0  |    1  |    1  |    1  |    1  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    1
   46   |    0  |    1  |    0  |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    1
   47   |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   48   |    0  |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    1
   49   |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    0  |    0

Edges:  |   39  |   40  |   41  |   42  |   43  |   44  |   45  |   46  |   47  |   48  |   49
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0
   1    |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   2    |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    0  |    0
   3    |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   4    |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1  |    1
   5    |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0
   6    |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    1
   7    |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    0
   8    |    0  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    1
   9    |    1  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0  |    0
   10   |    1  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0  |    0
   11   |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    1
   12   |    0  |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    0  |    0
   13   |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    1
   14   |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1
   15   |    1  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0
   16   |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    0
   17   |    0  |    1  |    0  |    0  |    0  |    1  |    0  |    1  |    0  |    1  |    1
   18   |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    1  |    0  |    1
   19   |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    1
   20   |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   21   |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    1  |    1  |    0  |    1
   22   |    1  |    1  |    1  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0
   23   |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   24   |    1  |    0  |    0  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1
   25   |    0  |    0  |    1  |    1  |    1  |    1  |    0  |    0  |    1  |    0  |    1
   26   |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0  |    0  |    0  |    0
   27   |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    1  |    1
   28   |    1  |    1  |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0
   29   |    0  |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0
   30   |    0  |    1  |    0  |    0  |    0  |    0  |    1  |    0  |    1  |    1  |    0
   31   |    1  |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    1
   32   |    0  |    0  |    1  |    0  |    0  |    1  |    1  |    1  |    0  |    0  |    0
   33   |    1  |    1  |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    0  |    1
   34   |    0  |    1  |    0  |    1  |    1  |    1  |    0  |    1  |    0  |    0  |    0
   35   |    0  |    0  |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    1  |    0
   36   |    1  |    1  |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    1
   37   |    0  |    0  |    1  |    0  |    1  |    1  |    0  |    1  |    0  |    0  |    0
   38   |    1  |    0  |    0  |    1  |    1  |    0  |    1  |    1  |    0  |    1  |    0
   39   |    -  |    0  |    1  |    0  |    1  |    0  |    1  |    0  |    0  |    0  |    1
   40   |    0  |    -  |    1  |    0  |    1  |    1  |    0  |    0  |    1  |    0  |    0
   41   |    1  |    1  |    -  |    0  |    0  |    1  |    0  |    0  |    1  |    0  |    0
   42   |    0  |    0  |    0  |    -  |    0  |    1  |    1  |    0  |    0  |    1  |    1
   43   |    1  |    1  |    0  |    0  |    -  |    0  |    0  |    1  |    1  |    1  |    1
   44   |    0  |    1  |    1  |    1  |    0  |    -  |    0  |    0  |    1  |    0  |    0
   45   |    1  |    0  |    0  |    1  |    0  |    0  |    -  |    0  |    1  |    0  |    0
   46   |    0  |    0  |    0  |    0  |    1  |    0  |    0  |    -  |    0  |    1  |    0
   47   |    0  |    1  |    1  |    0  |    1  |    1  |    1  |    0  |    -  |    1  |    0
   48   |    0  |    0  |    0  |    1  |    1  |    0  |    0  |    1  |    1  |    -  |    0
   49   |    1  |    0  |    0  |    1  |    1  |    0  |    0  |    0  |    0  |    0  |    -

Dist:   |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   10  |   11  |   12
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    -  |    0  |    0  |    0  |    0  |    0  |  6.2  |  2.1  |  9.8  |    0  |  3.6  |  1.7  |    0
   1    |    0  |    -  |  1.6  |  5.8  |  3.1  |  5.3  |  2.6  |    0  |  8.3  |    0  |  5.7  |  9.0  |    0
   2    |    0  |  1.6  |    -  |  3.3  |  9.2  |    0  |    0  |  2.1  |  2.6  |  5.1  |    0  |    0  |  3.7
   3    |    0  |  5.8  |  3.3  |    -  |    0  |  3.7  |  9.4  |  1.9  |  9.0  |  5.8  |  7.3  |  3.6  |  5.5
   4    |    0  |  3.1  |  9.2  |    0  |    -  |  9.6  |    0  |  5.7  |    0  |  6.2  |    0  |  1.8  |    0
   5    |    0  |  5.3  |    0  |  3.7  |  9.6  |    -  |  9.2  |  2.2  |  3.0  |    0  |  7.3  |    0  |  5.4
   6    |  6.2  |  2.6  |    0  |  9.4  |    0  |  9.2  |    -  |  5.4  |  9.0  |  5.0  |    0  |    0  |  4.6
   7    |  2.1  |    0  |  2.1  |  1.9  |  5.7  |  2.2  |  5.4  |    -  |    0  |    0  |    0  |  9.4  |  5.2
   8    |  9.8  |  8.3  |  2.6  |  9.0  |    0  |  3.0  |  9.0  |    0  |    -  |  1.5  |  8.1  |  3.4  |    0
   9    |    0  |    0  |  5.1  |  5.8  |  6.2  |    0  |  5.0  |    0  |  1.5  |    -  |    0  |    0  |  3.9
   10   |  3.6  |  5.7  |    0  |  7.3  |    0  |  7.3  |    0  |    0  |  8.1  |    0  |    -  |  4.6  |  7.9
   11   |  1.7  |  9.0  |    0  |  3.6  |  1.8  |    0  |    0  |  9.4  |  3.4  |    0  |  4.6  |    -  |  5.5
   12   |    0  |    0  |  3.7  |  5.5  |    0  |  5.4  |  4.6  |  5.2  |    0  |  3.9  |  7.9  |  5.5  |    -
   13   |  2.9  |  1.9  |    0  |    0  |    0  |    0  |  4.0  |  8.7  |    0  |  5.5  |  8.4  |    0  |  6.8
   14   |  1.7  |  9.4  |  2.1  |  2.4  |  6.0  |    0  |    0  |  3.5  |  9.9  |  8.5  |  4.2  |  2.9  |  4.7
   15   |    0  |    0  |    0  |    0  |  6.6  |  2.7  |  4.1  |    0  |  7.9  |  4.1  |    0  |  9.0  |  8.4
   16   |  9.4  |  5.6  |    0  |  7.7  |    0  |    0  |  7.0  |    0  |  6.4  |  1.5  |  1.5  |  3.9  |  5.5
   17   |    0  |  2.7  |    0  |  3.3  |    0  |  4.3  |    0  |  4.0  |  9.5  |  6.8  |    0  |    0  |  6.7
   18   |  6.9  |  7.6  |  2.8  |    0  |    0  |  5.6  |  2.4  |  3.4  |  6.1  |    0  |  3.4  |  6.1  |    0
   19   |    0  |  1.7  |  3.6  |  9.1  |  3.2  |  5.8  |  5.4  |  3.1  |    0  |  9.1  |    0  |    0  |  6.7
   20   |    0  |    0  |    0  |  9.3  |    0  |    0  |  5.8  |  6.0  |    0  |    0  |  8.4  |  5.0  |    0
   21   |  4.9  |  3.5  |  9.9  |  9.5  |  1.5  |  8.2  |  4.9  |  7.5  |  3.6  |  9.9  |  8.4  |    0  |  3.9
   22   |    0  |  2.2  |  4.2  |  9.3  |  3.5  |  1.2  |  2.5  |    0  |  7.6  |    0  |    0  |    0  |  4.2
   23   |  9.2  |    0  |  8.4  |    0  |  2.2  |  3.5  |  7.0  |  7.8  |    0  |    0  |  5.1  |    0  |  5.5
   24   |    0  |    0  |  1.0  |  7.0  |  4.0  |  8.3  |    0  |  7.6  |  7.4  |  7.9  |  1.4  |  2.4  |    0
   25   |  4.0  |  2.3  |  8.2  |    0  |    0  |  5.0  |    0  |    0  |    0  |    0  |    0  |  9.3  |  1.3
   26   |    0  |  1.9  |    0  |  1.2  |  2.3  |  5.6  |  4.2  |  2.6  |    0  |    0  |  9.8  |    0  |  5.8
   27   |    0  |    0  |    0  |  6.9  |  5.8  |  8.8  |    0  |    0  |  7.2  |    0  |    0  |    0  |  9.0
   28   |  6.7  |  9.4  |    0  |  4.1  |    0  |  6.7  |  8.3  |    0  |  5.9  |  7.1  |  5.9  |  2.3  |  7.7
   29   |    0  |  8.0  |  2.7  |  7.0  |    0  |  2.1  |  8.9  |    0  |  4.9  |    0  |    0  |  7.4  |    0
   30   |    0  |  7.7  |    0  |    0  |  3.5  |  8.2  |  8.9  |    0  |  4.2  |  6.9  |    0  |  7.3  |  9.2
   31   |  5.1  |    0  |  7.2  |    0  |  4.9  |    0  |    0  |  8.1  |  6.7  |    0  |    0  |  8.9  |  4.1
   32   |    0  |    0  |    0  |  3.2  |    0  |    0  |  2.3  |  4.2  |    0  |  5.1  |    0  |  7.2  |    0
   33   |  2.3  |  5.9  |    0  |    0  |  8.7  |  4.9  |    0  |  9.8  |  6.3  |    0  |  4.2  |    0  |  4.3
   34   |  7.1  |  1.0  |    0  |    0  |  1.6  |  3.8  |    0  |  9.8  |    0  |  9.6  |  6.3  |  7.3  |    0
   35   |    0  |    0  |  8.5  |  7.1  |    0  |    0  |    0  |    0  |  6.8  |  8.5  |  5.8  |  9.2  |  2.2
   36   |  9.0  |    0  |    0  |  6.2  |  3.5  |  1.8  |  7.6  |    0  |    0  |  5.8  |  9.4  |    0  |    0
   37   |  3.0  |  6.9  |  1.2  |  7.0  |  5.5  |  2.6  |  4.9  |  4.5  |  7.2  |    0  |  2.3  |  3.4  |    0
   38   |    0  |  9.0  |    0  |  9.7  |    0  |    0  |    0  |  3.0  |  7.5  |    0  |  7.8  |  6.3  |    0
   39   |    0  |  5.8  |  6.1  |  5.9  |  3.3  |  7.8  |  7.9  |    0  |    0  |  8.6  |  7.9  |  3.2  |    0
   40   |  2.3  |  9.6  |  6.2  |  7.4  |  6.9  |  1.1  |    0  |  6.9  |    0  |    0  |  7.7  |    0  |    0
   41   |  5.9  |    0  |    0  |    0  |  8.6  |    0  |  8.0  |  2.2  |  7.8  |    0  |  4.4  |  6.9  |  7.0
   42   |    0  |  8.0  |    0  |    0  |  2.6  |  4.3  |  7.4  |  4.3  |  2.1  |  3.9  |  1.5  |    0  |  8.0
   43   |    0  |    0  |  3.5  |    0  |  8.4  |  5.9  |    0  |    0  |  2.4  |  7.6  |  4.3  |  3.4  |    0
   44   |    0  |  4.3  |    0  |    0  |  7.7  |  8.9  |  9.8  |  5.8  |  3.2  |    0  |  4.6  |    0  |  7.0
   45   |    0  |  3.8  |    0  |    0  |    0  |  7.1  |  6.8  |    0  |  7.1  |  3.0  |    0  |  7.1  |  3.1
   46   |    0  |    0  |  5.6  |    0  |  2.2  |    0  |  7.2  |  3.3  |    0  |    0  |  6.7  |  9.3  |    0
   47   |  9.4  |  5.1  |  7.9  |  4.0  |    0  |    0  |  4.5  |    0  |    0  |  6.3  |  1.0  |  9.2  |  1.6
   48   |  1.7  |  8.1  |  8.7  |  3.2  |  6.8  |    0  |  9.7  |  7.8  |  2.8  |    0  |    0  |  5.9  |    0
   49   |  9.7  |    0  |    0  |  9.2  |  1.7  |  5.9  |  9.7  |  9.0  |  7.9  |  9.3  |  8.4  |  7.1  |    0

Dist:   |   13  |   14  |   15  |   16  |   17  |   18  |   19  |   20  |   21  |   22  |   23  |   24  |   25
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |  2.9  |  1.7  |    0  |  9.4  |    0  |  6.9  |    0  |    0  |  4.9  |    0  |  9.2  |    0  |  4.0
   1    |  1.9  |  9.4  |    0  |  5.6  |  2.7  |  7.6  |  1.7  |    0  |  3.5  |  2.2  |    0  |    0  |  2.3
   2    |    0  |  2.1  |    0  |    0  |    0  |  2.8  |  3.6  |    0  |  9.9  |  4.2  |  8.4  |  1.0  |  8.2
   3    |    0  |  2.4  |    0  |  7.7  |  3.3  |    0  |  9.1  |  9.3  |  9.5  |  9.3  |    0  |  7.0  |    0
   4    |    0  |  6.0  |  6.6  |    0  |    0  |    0  |  3.2  |    0  |  1.5  |  3.5  |  2.2  |  4.0  |    0
   5    |    0  |    0  |  2.7  |    0  |  4.3  |  5.6  |  5.8  |    0  |  8.2  |  1.2  |  3.5  |  8.3  |  5.0
   6    |  4.0  |    0  |  4.1  |  7.0  |    0  |  2.4  |  5.4  |  5.8  |  4.9  |  2.5  |  7.0  |    0  |    0
   7    |  8.7  |  3.5  |    0  |    0  |  4.0  |  3.4  |  3.1  |  6.0  |  7.5  |    0  |  7.8  |  7.6  |    0
   8    |    0  |  9.9  |  7.9  |  6.4  |  9.5  |  6.1  |    0  |    0  |  3.6  |  7.6  |    0  |  7.4  |    0
   9    |  5.5  |  8.5  |  4.1  |  1.5  |  6.8  |    0  |  9.1  |    0  |  9.9  |    0  |    0  |  7.9  |    0
   10   |  8.4  |  4.2  |    0  |  1.5  |    0  |  3.4  |    0  |  8.4  |  8.4  |    0  |  5.1  |  1.4  |    0
   11   |    0  |  2.9  |  9.0  |  3.9  |    0  |  6.1  |    0  |  5.0  |    0  |    0  |    0  |  2.4  |  9.3
   12   |  6.8  |  4.7  |  8.4  |  5.5  |  6.7  |    0  |  6.7  |    0  |  3.9  |  4.2  |  5.5  |    0  |  1.3
   13   |    -  |  7.2  |  4.2  |    0  |  9.2  |  1.2  |  7.5  |  1.2  |  4.1  |    0  |  7.7  |    0  |  9.9
   14   |  7.2  |    -  |    0  |  9.4  |  6.7  |  5.4  |  9.9  |    0  |    0  |    0  |  9.3  |  5.0  |  1.8
   15   |  4.2  |    0  |    -  |    0  |    0  |  3.0  |  4.1  |  1.7  |  2.4  |    0  |  2.3  |  8.0  |    0
   16   |    0  |  9.4  |    0  |    -  |  6.6  |    0  |  9.7  |  9.1  |  1.1  |  1.4  |  8.0  |  9.9  |    0
   17   |  9.2  |  6.7  |    0  |  6.6  |    -  |  7.5  |  3.2  |  8.8  |    0  |    0  |  6.6  |  3.0  |  6.9
   18   |  1.2  |  5.4  |  3.0  |    0  |  7.5  |    -  |  8.3  |  6.4  |    0  |  8.2  |    0  |  1.7  |    0
   19   |  7.5  |  9.9  |  4.1  |  9.7  |  3.2  |  8.3  |    -  |  9.0  |    0  |    0  |  9.6  |  8.0  |  8.9
   20   |  1.2  |    0  |  1.7  |  9.1  |  8.8  |  6.4  |  9.0  |    -  |  7.6  |  9.7  |    0  |  4.5  |  9.7
   21   |  4.1  |    0  |  2.4  |  1.1  |    0  |    0  |    0  |  7.6  |    -  |  9.6  |  1.9  |  1.1  |    0
   22   |    0  |    0  |    0  |  1.4  |    0  |  8.2  |    0  |  9.7  |  9.6  |    -  |    0  |  6.0  |  2.4
   23   |  7.7  |  9.3  |  2.3  |  8.0  |  6.6  |    0  |  9.6  |    0  |  1.9  |    0  |    -  |  1.6  |  1.5
   24   |    0  |  5.0  |  8.0  |  9.9  |  3.0  |  1.7  |  8.0  |  4.5  |  1.1  |  6.0  |  1.6  |    -  |  3.0
   25   |  9.9  |  1.8  |    0  |    0  |  6.9  |    0  |  8.9  |  9.7  |    0  |  2.4  |  1.5  |  3.0  |    -
   26   |  4.7  |    0  |    0  |  8.7  |    0  |    0  |  8.7  |  9.0  |  4.7  |  4.6  |  7.7  |  7.4  |  7.0
   27   |    0  |    0  |  6.8  |    0  |  4.7  |  9.3  |    0  |    0  |    0  |  8.4  |    0  |  1.3  |    0
   28   |  9.6  |  8.9  |  7.5  |    0  |  6.2  |    0  |    0  |  6.0  |  1.8  |    0  |  1.6  |  8.0  |  2.8
   29   |  6.6  |  5.9  |    0  |    0  |    0  |  5.4  |  3.4  |  4.8  |    0  |  2.2  |  5.1  |    0  |  2.8
   30   |  6.2  |  6.3  |    0  |  7.3  |  1.2  |  1.8  |    0  |  7.9  |    0  |  2.2  |  6.3  |  4.8  |    0
   31   |  2.1  |  4.0  |  6.2  |  6.2  |    0  |  3.3  |  2.1  |  5.2  |  6.2  |    0  |    0  |    0  |    0
   32   |  7.3  |  4.1  |  7.1  |  6.3  |  5.8  |  4.3  |    0  |  4.2  |  9.2  |    0  |  9.8  |  7.1  |  6.8
   33   |    0  |  4.3  |  6.9  |  3.2  |  3.8  |    0  |  5.7  |  2.0  |  1.4  |  2.2  |    0  |  2.1  |  6.0
   34   |    0  |    0  |  8.4  |  7.5  |    0  |    0  |  2.4  |    0  |  6.9  |  3.4  |  1.2  |    0  |  9.3
   35   |  7.4  |  6.5  |    0  |  5.8  |    0  |    0  |  1.7  |  8.8  |  1.1  |  8.2  |  3.7  |  8.3  |  3.9
   36   |  8.4  |  7.7  |  2.8  |  6.7  |  8.5  |  9.0  |    0  |  6.9  |  2.7  |    0  |    0  |    0  |    0
   37   |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |    0  |  7.0  |  8.6  |    0
   38   |  4.1  |  7.1  |  3.8  |  5.7  |  3.8  |  7.9  |    0  |    0  |    0  |  5.9  |  2.2  |    0  |  6.0
   39   |  8.6  |  8.8  |  5.6  |  1.0  |  7.0  |  9.7  |  4.6  |    0  |  7.7  |  3.2  |  2.7  |  5.9  |    0
   40   |  6.0  |    0  |    0  |  9.8  |  4.0  |  2.2  |    0  |    0  |  3.0  |  1.0  |    0  |    0  |    0
   41   |    0  |    0  |    0  |  1.4  |    0  |    0  |    0  |  3.7  |    0  |  3.0  |  5.1  |    0  |  7.4
   42   |  5.8  |    0  |  2.4  |  7.3  |    0  |  2.5  |  7.8  |  6.1  |  5.4  |  6.7  |    0  |    0  |  2.3
   43   |  7.8  |    0  |  7.5  |    0  |    0  |    0  |  9.4  |  1.7  |  9.7  |    0  |  2.2  |  9.3  |  4.1
   44   |  2.2  |  5.6  |  6.7  |  1.6  |  5.8  |  1.7  |  9.8  |    0  |  8.5  |    0  |  8.9  |    0  |  4.0
   45   |  4.7  |    0  |    0  |  3.7  |    0  |  9.5  |    0  |  1.6  |  2.1  |    0  |    0  |  1.2  |    0
   46   |    0  |    0  |  1.6  |  6.2  |  3.4  |  7.6  |  3.3  |    0  |  5.8  |    0  |  6.0  |  2.2  |    0
   47   |    0  |  9.5  |  2.7  |    0  |    0  |  9.7  |  9.1  |  7.8  |  2.8  |  8.0  |  8.8  |    0  |  1.3
   48   |    0  |    0  |  4.9  |  3.4  |  1.6  |  4.0  |  5.6  |    0  |    0  |  2.5  |    0  |    0  |  7.1
   49   |  8.7  |  5.0  |    0  |    0  |  3.6  |  8.1  |  3.4  |  3.6  |  6.1  |    0  |  6.2  |  9.0  |  8.2

Dist:   |   26  |   27  |   28  |   29  |   30  |   31  |   32  |   33  |   34  |   35  |   36  |   37  |   38
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |    0  |  6.7  |    0  |    0  |  5.1  |    0  |  2.3  |  7.1  |    0  |  9.0  |  3.0  |    0
   1    |  1.9  |    0  |  9.4  |  8.0  |  7.7  |    0  |    0  |  5.9  |  1.0  |    0  |    0  |  6.9  |  9.0
   2    |    0  |    0  |    0  |  2.7  |    0  |  7.2  |    0  |    0  |    0  |  8.5  |    0  |  1.2  |    0
   3    |  1.2  |  6.9  |  4.1  |  7.0  |    0  |    0  |  3.2  |    0  |    0  |  7.1  |  6.2  |  7.0  |  9.7
   4    |  2.3  |  5.8  |    0  |    0  |  3.5  |  4.9  |    0  |  8.7  |  1.6  |    0  |  3.5  |  5.5  |    0
   5    |  5.6  |  8.8  |  6.7  |  2.1  |  8.2  |    0  |    0  |  4.9  |  3.8  |    0  |  1.8  |  2.6  |    0
   6    |  4.2  |    0  |  8.3  |  8.9  |  8.9  |    0  |  2.3  |    0  |    0  |    0  |  7.6  |  4.9  |    0
   7    |  2.6  |    0  |    0  |    0  |    0  |  8.1  |  4.2  |  9.8  |  9.8  |    0  |    0  |  4.5  |  3.0
   8    |    0  |  7.2  |  5.9  |  4.9  |  4.2  |  6.7  |    0  |  6.3  |    0  |  6.8  |    0  |  7.2  |  7.5
   9    |    0  |    0  |  7.1  |    0  |  6.9  |    0  |  5.1  |    0  |  9.6  |  8.5  |  5.8  |    0  |    0
   10   |  9.8  |    0  |  5.9  |    0  |    0  |    0  |    0  |  4.2  |  6.3  |  5.8  |  9.4  |  2.3  |  7.8
   11   |    0  |    0  |  2.3  |  7.4  |  7.3  |  8.9  |  7.2  |    0  |  7.3  |  9.2  |    0  |  3.4  |  6.3
   12   |  5.8  |  9.0  |  7.7  |    0  |  9.2  |  4.1  |    0  |  4.3  |    0  |  2.2  |    0  |    0  |    0
   13   |  4.7  |    0  |  9.6  |  6.6  |  6.2  |  2.1  |  7.3  |    0  |    0  |  7.4  |  8.4  |    0  |  4.1
   14   |    0  |    0  |  8.9  |  5.9  |  6.3  |  4.0  |  4.1  |  4.3  |    0  |  6.5  |  7.7  |    0  |  7.1
   15   |    0  |  6.8  |  7.5  |    0  |    0  |  6.2  |  7.1  |  6.9  |  8.4  |    0  |  2.8  |    0  |  3.8
   16   |  8.7  |    0  |    0  |    0  |  7.3  |  6.2  |  6.3  |  3.2  |  7.5  |  5.8  |  6.7  |    0  |  5.7
   17   |    0  |  4.7  |  6.2  |    0  |  1.2  |    0  |  5.8  |  3.8  |    0  |    0  |  8.5  |    0  |  3.8
   18   |    0  |  9.3  |    0  |  5.4  |  1.8  |  3.3  |  4.3  |    0  |    0  |    0  |  9.0  |    0  |  7.9
   19   |  8.7  |    0  |    0  |  3.4  |    0  |  2.1  |    0  |  5.7  |  2.4  |  1.7  |    0  |    0  |    0
   20   |  9.0  |    0  |  6.0  |  4.8  |  7.9  |  5.2  |  4.2  |  2.0  |    0  |  8.8  |  6.9  |    0  |    0
   21   |  4.7  |    0  |  1.8  |    0  |    0  |  6.2  |  9.2  |  1.4  |  6.9  |  1.1  |  2.7  |    0  |    0
   22   |  4.6  |  8.4  |    0  |  2.2  |  2.2  |    0  |    0  |  2.2  |  3.4  |  8.2  |    0  |    0  |  5.9
   23   |  7.7  |    0  |  1.6  |  5.1  |  6.3  |    0  |  9.8  |    0  |  1.2  |  3.7  |    0  |  7.0  |  2.2
   24   |  7.4  |  1.3  |  8.0  |    0  |  4.8  |    0  |  7.1  |  2.1  |    0  |  8.3  |    0  |  8.6  |    0
   25   |  7.0  |    0  |  2.8  |  2.8  |    0  |    0  |  6.8  |  6.0  |  9.3  |  3.9  |    0  |    0  |  6.0
   26   |    -  |  4.5  |  6.9  |  5.3  |  2.5  |  3.0  |  9.4  |  9.1  |  7.1  |  2.4  |  8.0  |  8.4  |    0
   27   |  4.5  |    -  |  1.3  |  2.9  |  2.6  |    0  |  5.3  |  4.2  |    0  |    0  |  3.4  |  8.7  |  2.8
   28   |  6.9  |  1.3  |    -  |  4.4  |  8.4  |    0  |  4.1  |    0  |  8.9  |  8.2  |  4.6  |  3.9  |  1.0
   29   |  5.3  |  2.9  |  4.4  |    -  |  1.4  |  1.6  |  4.1  |    0  |    0  |    0  |  9.8  |    0  |  9.9
   30   |  2.5  |  2.6  |  8.4  |  1.4  |    -  |  7.2  |  8.8  |  9.2  |  3.9  |  2.9  |  5.1  |  7.1  |    0
   31   |  3.0  |    0  |    0  |  1.6  |  7.2  |    -  |  5.3  |    0  |  7.7  |  3.5  |    0  |  5.7  |  4.6
   32   |  9.4  |  5.3  |  4.1  |  4.1  |  8.8  |  5.3  |    -  |    0  |    0  |  1.7  |  5.6  |  6.0  |  8.4
   33   |  9.1  |  4.2  |    0  |    0  |  9.2  |    0  |    0  |    -  |  1.4  |  7.8  |  9.8  |  6.2  |    0
   34   |  7.1  |    0  |  8.9  |    0  |  3.9  |  7.7  |    0  |  1.4  |    -  |  5.4  |    0  |  8.0  |    0
   35   |  2.4  |    0  |  8.2  |    0  |  2.9  |  3.5  |  1.7  |  7.8  |  5.4  |    -  |  3.3  |    0  |  5.6
   36   |  8.0  |  3.4  |  4.6  |  9.8  |  5.1  |    0  |  5.6  |  9.8  |    0  |  3.3  |    -  |  7.3  |    0
   37   |  8.4  |  8.7  |  3.9  |    0  |  7.1  |  5.7  |  6.0  |  6.2  |  8.0  |    0  |  7.3  |    -  |  2.7
   38   |    0  |  2.8  |  1.0  |  9.9  |    0  |  4.6  |  8.4  |    0  |    0  |  5.6  |    0  |  2.7  |    -
   39   |    0  |  8.7  |  7.5  |  2.5  |    0  |  8.1  |    0  |  2.6  |  7.2  |    0  |  7.1  |  7.5  |  4.1
   40   |  3.5  |    0  |  3.8  |    0  |  4.1  |    0  |    0  |  9.9  |  4.3  |    0  |  9.3  |  8.5  |    0
   41   |  9.2  |    0  |  5.2  |  8.9  |    0  |    0  |  9.7  |    0  |  3.8  |    0  |    0  |  9.8  |  2.4
   42   |  1.8  |  8.8  |    0  |  6.2  |  7.8  |  9.2  |    0  |  5.0  |  3.1  |    0  |  6.5  |    0  |  9.0
   43   |  1.8  |    0  |  3.0  |  9.5  |    0  |    0  |    0  |  7.0  |  3.2  |  2.1  |  1.1  |  6.2  |  3.9
   44   |  5.7  |    0  |  6.5  |  9.7  |    0  |  4.1  |  6.5  |    0  |  9.0  |  4.0  |  4.0  |  9.4  |  5.0
   45   |    0  |  3.7  |  7.4  |  2.1  |  7.1  |  1.7  |  8.1  |  7.8  |    0  |  4.9  |  5.9  |    0  |  9.6
   46   |    0  |  5.6  |  4.8  |  8.6  |    0  |  4.6  |  1.5  |  3.4  |  5.5  |  2.9  |    0  |  3.0  |  6.3
   47   |    0  |  5.5  |    0  |  7.4  |  6.5  |  4.9  |  6.8  |    0  |    0  |    0  |    0  |    0  |    0
   48   |    0  |  9.2  |    0  |    0  |  7.5  |  2.1  |    0  |  1.1  |  5.8  |  1.2  |  2.4  |    0  |  8.7
   49   |    0  |  2.4  |  3.7  |  4.6  |  2.3  |  2.1  |    0  |  3.6  |    0  |  2.3  |  9.9  |    0  |    0

Dist:   |   39  |   40  |   41  |   42  |   43  |   44  |   45  |   46  |   47  |   48  |   49
--------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------
   0    |    0  |  2.3  |  5.9  |    0  |    0  |    0  |    0  |    0  |  9.4  |  1.7  |  9.7
   1    |  5.8  |  9.6  |    0  |  8.0  |    0  |  4.3  |  3.8  |    0  |  5.1  |  8.1  |    0
   2    |  6.1  |  6.2  |    0  |    0  |  3.5  |    0  |    0  |  5.6  |  7.9  |  8.7  |    0
   3    |  5.9  |  7.4  |    0  |    0  |    0  |    0  |    0  |    0  |  4.0  |  3.2  |  9.2
   4    |  3.3  |  6.9  |  8.6  |  2.6  |  8.4  |  7.7  |    0  |  2.2  |    0  |  6.8  |  1.7
   5    |  7.8  |  1.1  |    0  |  4.3  |  5.9  |  8.9  |  7.1  |    0  |    0  |    0  |  5.9
   6    |  7.9  |    0  |  8.0  |  7.4  |    0  |  9.8  |  6.8  |  7.2  |  4.5  |  9.7  |  9.7
   7    |    0  |  6.9  |  2.2  |  4.3  |    0  |  5.8  |    0  |  3.3  |    0  |  7.8  |  9.0
   8    |    0  |    0  |  7.8  |  2.1  |  2.4  |  3.2  |  7.1  |    0  |    0  |  2.8  |  7.9
   9    |  8.6  |    0  |    0  |  3.9  |  7.6  |    0  |  3.0  |    0  |  6.3  |    0  |  9.3
   10   |  7.9  |  7.7  |  4.4  |  1.5  |  4.3  |  4.6  |    0  |  6.7  |  1.0  |    0  |  8.4
   11   |  3.2  |    0  |  6.9  |    0  |  3.4  |    0  |  7.1  |  9.3  |  9.2  |  5.9  |  7.1
   12   |    0  |    0  |  7.0  |  8.0  |    0  |  7.0  |  3.1  |    0  |  1.6  |    0  |    0
   13   |  8.6  |  6.0  |    0  |  5.8  |  7.8  |  2.2  |  4.7  |    0  |    0  |    0  |  8.7
   14   |  8.8  |    0  |    0  |    0  |    0  |  5.6  |    0  |    0  |  9.5  |    0  |  5.0
   15   |  5.6  |    0  |    0  |  2.4  |  7.5  |  6.7  |    0  |  1.6  |  2.7  |  4.9  |    0
   16   |  1.0  |  9.8  |  1.4  |  7.3  |    0  |  1.6  |  3.7  |  6.2  |    0  |  3.4  |    0
   17   |  7.0  |  4.0  |    0  |    0  |    0  |  5.8  |    0  |  3.4  |    0  |  1.6  |  3.6
   18   |  9.7  |  2.2  |    0  |  2.5  |    0  |  1.7  |  9.5  |  7.6  |  9.7  |  4.0  |  8.1
   19   |  4.6  |    0  |    0  |  7.8  |  9.4  |  9.8  |    0  |  3.3  |  9.1  |  5.6  |  3.4
   20   |    0  |    0  |  3.7  |  6.1  |  1.7  |    0  |  1.6  |    0  |  7.8  |    0  |  3.6
   21   |  7.7  |  3.0  |    0  |  5.4  |  9.7  |  8.5  |  2.1  |  5.8  |  2.8  |    0  |  6.1
   22   |  3.2  |  1.0  |  3.0  |  6.7  |    0  |    0  |    0  |    0  |  8.0  |  2.5  |    0
   23   |  2.7  |    0  |  5.1  |    0  |  2.2  |  8.9  |    0  |  6.0  |  8.8  |    0  |  6.2
   24   |  5.9  |    0  |    0  |    0  |  9.3  |    0  |  1.2  |  2.2  |    0  |    0  |  9.0
   25   |    0  |    0  |  7.4  |  2.3  |  4.1  |  4.0  |    0  |    0  |  1.3  |  7.1  |  8.2
   26   |    0  |  3.5  |  9.2  |  1.8  |  1.8  |  5.7  |    0  |    0  |    0  |    0  |    0
   27   |  8.7  |    0  |    0  |  8.8  |    0  |    0  |  3.7  |  5.6  |  5.5  |  9.2  |  2.4
   28   |  7.5  |  3.8  |  5.2  |    0  |  3.0  |  6.5  |  7.4  |  4.8  |    0  |    0  |  3.7
   29   |  2.5  |    0  |  8.9  |  6.2  |  9.5  |  9.7  |  2.1  |  8.6  |  7.4  |    0  |  4.6
   30   |    0  |  4.1  |    0  |  7.8  |    0  |    0  |  7.1  |    0  |  6.5  |  7.5  |  2.3
   31   |  8.1  |    0  |    0  |  9.2  |    0  |  4.1  |  1.7  |  4.6  |  4.9  |  2.1  |  2.1
   32   |    0  |    0  |  9.7  |    0  |    0  |  6.5  |  8.1  |  1.5  |  6.8  |    0  |    0
   33   |  2.6  |  9.9  |    0  |  5.0  |  7.0  |    0  |  7.8  |  3.4  |    0  |  1.1  |  3.6
   34   |  7.2  |  4.3  |  3.8  |  3.1  |  3.2  |  9.0  |    0  |  5.5  |    0  |  5.8  |    0
   35   |    0  |    0  |    0  |    0  |  2.1  |  4.0  |  4.9  |  2.9  |    0  |  1.2  |  2.3
   36   |  7.1  |  9.3  |    0  |  6.5  |  1.1  |  4.0  |  5.9  |    0  |    0  |  2.4  |  9.9
   37   |  7.5  |  8.5  |  9.8  |    0  |  6.2  |  9.4  |    0  |  3.0  |    0  |    0  |    0
   38   |  4.1  |    0  |  2.4  |  9.0  |  3.9  |  5.0  |  9.6  |  6.3  |    0  |  8.7  |    0
   39   |    -  |  3.8  |  6.8  |    0  |  5.6  |    0  |  7.3  |    0  |    0  |  4.6  |  5.5
   40   |  3.8  |    -  |  8.5  |  4.9  |  4.9  |  9.5  |    0  |    0  |  6.5  |  5.0  |    0
   41   |  6.8  |  8.5  |    -  |    0  |  9.9  |  4.6  |    0  |    0  |  6.8  |    0  |  1.3
   42   |    0  |  4.9  |    0  |    -  |  5.1  |  7.8  |  5.6  |    0  |    0  |  7.9  |  8.7
   43   |  5.6  |  4.9  |  9.9  |  5.1  |    -  |    0  |    0  |  8.1  |  9.8  |  6.0  |  4.6
   44   |    0  |  9.5  |  4.6  |  7.8  |    0  |    -  |    0  |  5.7  |  4.9  |  1.9  |    0
   45   |  7.3  |    0  |    0  |  5.6  |    0  |    0  |    -  |  2.0  |  6.7  |    0  |    0
   46   |    0  |    0  |    0  |    0  |  8.1  |  5.7  |  2.0  |    -  |    0  |  7.2  |    0
   47   |    0  |  6.5  |  6.8  |    0  |  9.8  |  4.9  |  6.7  |    0  |    -  |  9.3  |  4.6
   48   |  4.6  |  5.0  |    0  |  7.9  |  6.0  |  1.9  |    0  |  7.2  |  9.3  |    -  |    0
   49   |  5.5  |    0  |  1.3  |  8.7  |  4.6  |    0  |    0  |    0  |  4.6  |    0  |    -

Dijkstras Algorithm Nearest City Paths and Distance to Origin:
(  origin  )
( 11 | 1.7 )  ->  (  origin  )
( 48 | 1.7 )  ->  (  origin  )
(  7 | 2.1 )  ->  (  origin  )
( 40 | 2.3 )  ->  (  origin  )
( 33 | 2.3 )  ->  (  origin  )
( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 37 | 3.0 )  ->  (  origin  )
( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 17 | 3.3 )  ->  ( 48 | 1.7 )  ->  (  origin  )
(  5 | 3.4 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 10 | 3.6 )  ->  (  origin  )
( 34 | 3.7 )  ->  ( 33 | 2.3 )  ->  (  origin  )
(  3 | 4.0 )  ->  (  7 | 2.1 )  ->  (  origin  )
( 21 | 4.0 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 36 | 4.1 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 24 | 4.1 )  ->  ( 11 | 1.7 )  ->  (  origin  )
(  2 | 4.2 )  ->  ( 37 | 3.0 )  ->  (  origin  )
( 20 | 4.3 )  ->  ( 33 | 2.3 )  ->  (  origin  )
(  8 | 4.5 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 30 | 4.5 )  ->  ( 17 | 3.3 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 19 | 4.6 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 32 | 4.6 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
(  1 | 4.7 )  ->  ( 34 | 3.7 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 16 | 4.7 )  ->  ( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 39 | 4.9 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 31 | 5.1 )  ->  (  origin  )
( 43 | 5.1 )  ->  ( 11 | 1.7 )  ->  (  origin  )
( 26 | 5.2 )  ->  (  3 | 4.0 )  ->  (  7 | 2.1 )  ->  (  origin  )
(  4 | 5.3 )  ->  ( 34 | 3.7 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 45 | 5.3 )  ->  ( 24 | 4.1 )  ->  ( 11 | 1.7 )  ->  (  origin  )
( 46 | 5.4 )  ->  (  7 | 2.1 )  ->  (  origin  )
( 13 | 5.5 )  ->  ( 20 | 4.3 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 18 | 5.5 )  ->  (  7 | 2.1 )  ->  (  origin  )
( 29 | 5.5 )  ->  ( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 14 | 5.6 )  ->  (  7 | 2.1 )  ->  (  origin  )
( 25 | 5.7 )  ->  ( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 28 | 5.8 )  ->  ( 21 | 4.0 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
(  6 | 5.8 )  ->  ( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 49 | 5.9 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 23 | 5.9 )  ->  ( 21 | 4.0 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
(  9 | 6.0 )  ->  (  8 | 4.5 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 15 | 6.0 )  ->  ( 20 | 4.3 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 41 | 6.1 )  ->  ( 16 | 4.7 )  ->  ( 22 | 3.3 )  ->  ( 40 | 2.3 )  ->  (  origin  )
( 42 | 6.4 )  ->  (  7 | 2.1 )  ->  (  origin  )
( 12 | 6.6 )  ->  ( 33 | 2.3 )  ->  (  origin  )
( 47 | 6.8 )  ->  ( 21 | 4.0 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 38 | 6.8 )  ->  ( 28 | 5.8 )  ->  ( 21 | 4.0 )  ->  ( 35 | 2.9 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 27 | 7.1 )  ->  ( 30 | 4.5 )  ->  ( 17 | 3.3 )  ->  ( 48 | 1.7 )  ->  (  origin  )
( 44 | 7.7 )  ->  ( 13 | 5.5 )  ->  ( 20 | 4.3 )  ->  ( 33 | 2.3 )  ->  (  origin  )
(connected cities 50 | avg path length 4.75 )


*************************************************
*   Results of Randomly Generated City Graphs   *
*      size     |     density   |     avg dist  *
*       50      |       20      |       6.39    *
*       50      |       40      |       4.75    *
*************************************************

City Graph with size 50 and density 40 deleted.

City Graph with size 50 and density 20 deleted.

City Graph with size 9 deleted.


C:\Programming\3.C++forCProgrammers\HW2_DijkstraAlgorithm>
* 
* 
*/





