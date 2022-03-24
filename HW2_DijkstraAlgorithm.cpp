// 
// C++ for C Programmers Part 1
// Week 3 Peer Graded Assignment Homework
//
// Dijkstra's Algorithm
// 
// Connectivity Matrices
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
		cout << "Test Data City Graph with size " << size_ << endl;
		InitializeCityMatrices();
		//test dataset for half matrix
		cout << "Implementing graph shown here https ://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/" << endl;
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
		//print connectivity matrix
		//PrintCityConnectivityMatrix();
		//PrintCityDistanceMatrix();
		cout << "Generated City Graph with size " << size_ << " and density " << density_ << endl;
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
	const int maxColsToShowPerPage = 13;
	void PrintCityConnectivityMatrix()
	{
		int pages = size_ / maxColsToShowPerPage;
		for (int p = 0; p <= pages; p++)
		{
			cout << "Edges:\t";
			for (int i = p * maxColsToShowPerPage; i < min(size_ , (p + 1) * maxColsToShowPerPage); i++)
				cout << "|   " << i << "\t";
			cout << endl << "--------";
			for (int i = p * maxColsToShowPerPage; i < min(size_, (p + 1) * maxColsToShowPerPage); i++)
				cout << "|-------";
			cout << endl;
			for (int i = 0; i < size_; i++) 
			{
				cout << "   " << i << "\t";
				for (int j = p * maxColsToShowPerPage; j < min(size_, (p + 1) * maxColsToShowPerPage); j++)
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
		int pages = size_ / maxColsToShowPerPage;
		for (int p = 0; p <= pages; p++)
		{
			cout << "Dist:\t";
			for (int i = p * maxColsToShowPerPage; i < min(size_, (p + 1) * maxColsToShowPerPage); i++)
				cout << "|   " << i << "\t";
			cout << endl << "--------";
			for (int i = p * maxColsToShowPerPage; i < min(size_, (p + 1) * maxColsToShowPerPage); i++)
				cout << "|-------";
			cout << endl;
			for (int i = 0; i < size_; i++) 
			{
				cout << "   " << i << "\t";
				for (int j = p * maxColsToShowPerPage; j < min(size_, (p + 1) * maxColsToShowPerPage); j++) 
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
	printf(" | avg path length %0.2f )\n", this->avg_dist_);
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