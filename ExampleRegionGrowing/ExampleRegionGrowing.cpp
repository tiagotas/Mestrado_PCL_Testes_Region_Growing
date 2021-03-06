// ExampleRegionGrowing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	pcl::search::Search <pcl::PointXYZRGB>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZRGB> >(new pcl::search::KdTree<pcl::PointXYZRGB>);

	pcl::PointCloud <pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud <pcl::PointXYZRGB>);
	


	std::cout << "Iniciando Leitura da Nuvem de Pontos... ";	
	if (pcl::io::loadPCDFile <pcl::PointXYZRGB>("paciente.pcd", *cloud) == -1)
	{
		std::cout << "Cloud reading failed." << std::endl;
		return (-1);
	}
	std::cout << "OK!" << std::endl << std::endl;

	



	/*pcl::IndicesPtr indices (new std::vector <int>);
	pcl::PassThrough<pcl::PointXYZRGB> pass;
	pass.setInputCloud(cloud);
	pass.setFilterFieldName("z");
	pass.setFilterLimits(0.0, 1.0);
	pass.filter(*indices);*/


	//std::cout << "Valor para Distance Threshold: ";
	//std::cout << "Valor para Point Color Threshold: ";
	//std::cout << "Valor para Region Color Threshold: ";


	std::cout << "Definindo Parametros de Segmentacao... ";
	pcl::RegionGrowingRGB<pcl::PointXYZRGB> reg;
	reg.setInputCloud(cloud);
	//reg.setIndices(indices);	
	reg.setSearchMethod(tree);	
	reg.setDistanceThreshold(1); //1 mesmo
	reg.setPointColorThreshold(4.0); // estava 3
	reg.setRegionColorThreshold(3);	// estava 3
	reg.setMinClusterSize(500); //estava 600 | desci de 1000 para 800 para seguimentar o braco.
	std::cout << "OK!" << std::endl << std::endl;


	/*
	
	Quando os valores 

	reg.setPointColorThreshold(8); 
	reg.setRegionColorThreshold(8);	
	
	Toda nuvem de pontos foi segmentada com uma só cor!
	________________________________________________________________________________
	Quando o valor de 
	reg.setPointColorThreshold(3); 
	A nuvem segmentou com 53 clusters.
	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(7); a nuvem segmentou em 8
	clusters, fundiu toalha e regiões de interesse.
	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(6.5); a nuvem segmentou em 7
	clusters, fundiu toalha e regiões de interesse.

	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(6.5); a nuvem segmentou em 10
	clusters, fundiu toalha e regiões de interesse.
	reg.setMinClusterSize(1100);
	reg.setRegionColorThreshold(3);

	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(4.5); a nuvem segmentou em 27
	clusters, demarcou pernas, tolhas, tronco e faltaram reigões de interesse no ombro.
	reg.setMinClusterSize(1100);
	reg.setRegionColorThreshold(3

	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(4.0); a nuvem segmentou em 30
	clusters, demarcou pernas, tolhas, tronco e faltaram reigões de interesse no ombro.
	reg.setMinClusterSize(1100);
	reg.setRegionColorThreshold(3);

	________________________________________________________________________________
	Quando o valor de reg.setPointColorThreshold(4.0); a nuvem segmentou em 33
	clusters, demarcou pernas, tolhas, tronco e faltaram reigões de interesse no ombro.
	reg.setMinClusterSize(1000);
	reg.setRegionColorThreshold(3);


	________________________________________________________________________________
	Gerou 63 clusters com a alteração do reg.setRegionColorThreshold(1) (antes estava 4).
	Apenas fragmentou mais. Gerou regiões mais pequenas por causa do min clusters size

	reg.setDistanceThreshold(1); //1 mesmo
	reg.setPointColorThreshold(4.0); // estava 3
	reg.setRegionColorThreshold(1);	// estava 3
	reg.setMinClusterSize(600); //estava 600


	________________________________________________________________________________
	Gerou 46 clusters com a alteração do reg.setRegionColorThreshold(8) (antes estava 3).
	Apenas fragmentou mais.

	reg.setDistanceThreshold(1); //1 mesmo
	reg.setPointColorThreshold(4.0); // estava 3
	reg.setRegionColorThreshold(8);	// estava 3
	reg.setMinClusterSize(1000); //estava 600




	
	*/




	std::cout << "Extraindo os clusters (vai demorar)... ";
	std::vector <pcl::PointIndices> clusters;
	reg.extract(clusters);
	std::cout << "OK!" << std::endl << std::endl;
	std::cout << "Gerou " << clusters.size() << " Clusters" << std::endl << std::endl;
	

	std::cout << "Segmentacao Finzalida." << std::endl << std::endl;


	std::cout << "Tentando gravar nuvem segmentada no PLY...";
	pcl::PointCloud <pcl::PointXYZRGB>::Ptr colored_cloud = reg.getColoredCloud();
	
	std::string nome_arquivo_segmentado = "paciente_segmentado_clusters_" + std::to_string(clusters.size());
	nome_arquivo_segmentado = nome_arquivo_segmentado + ".ply";
	
	pcl::io::savePLYFile(nome_arquivo_segmentado, *colored_cloud);
	std::cout << "OK!" << std::endl << std::endl;





	std::cout << "Deseja Gerar os Arquivos de cada Cluster separadamente? S/N" << std::endl;
	std::string opcao;
	getline(std::cin, opcao);


	if (opcao == "S") {
		std::cout << std::endl << "Iniciando Geração de Arquivos Separados de Cada Cluster..." << std::endl << std::endl;

		pcl::PointCloud<pcl::PointXYZRGB> cloud_w;

		//pcl::PCDWriter writer;

		pcl::PLYWriter writer;

		for (int ii = 0; ii < clusters.size(); ii++)
		{
			cloud_w.clear();
			for (size_t i = 0; i < clusters[ii].indices.size(); ++i)
			{
				cloud_w.push_back(cloud->points[clusters[ii].indices[i]]);
			}

			std::stringstream ss;
			ss << "paciente_segmentado_cluster_" << ii << ".ply";
			writer.write<pcl::PointXYZRGB>(ss.str(), cloud_w, false);
			std::cerr << "Saved " << cloud_w.points.size() << " data points to file	" << ii << std::endl;
		}


		std::cout << "Geração dos Arquivos dos Clusters Concluida!" << std::endl << std::endl;

	}


	std::cout << "Execucao Finalizada." << std::endl;

	getchar();

	return (0);
}

