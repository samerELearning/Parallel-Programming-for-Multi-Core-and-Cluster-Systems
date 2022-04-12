#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <accelmath.h>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include "dcdread.h"
#include <assert.h>
#include <omp.h>
#include <nvtx3/nvToolsExt.h>

void pair_gpu(const double *d_x, const double *d_y, const double *d_z,
			  unsigned long long int *d_g2, int numatm, int nconf,
			  const double xbox, const double ybox, const double zbox,
			  int d_bin);

int main(int argc, char *argv[])
{
	double xbox, ybox, zbox;
	double *h_x, *h_y, *h_z;
	unsigned long long int *h_g2;
	int nbin;
	int numatm, nconf, inconf;
	string file;
	

	inconf = 10;
	nbin = 2000;
	file = "../input/alk.traj.dcd";
	std::ifstream infile;
	infile.open(file.c_str());
	if (!infile)
	{
		cout << "file " << file.c_str() << " not found\n";
		return 1;
	}
	assert(infile);

	ofstream pairfile, stwo;
	pairfile.open("RDF.dat");
	stwo.open("Pair_entropy.dat");

	dcdreadhead(&numatm, &nconf, infile);
	cout << "Dcd file has " << numatm << " atoms and " << nconf << " frames" << endl;
	if (inconf > nconf)
		cout << "nconf is reset to " << nconf << endl;
	else
	{
		nconf = inconf;
	}
	cout << "Calculating RDF for " << nconf << " frames" << endl;

	unsigned long long int sizef = nconf * numatm * sizeof(double);
	unsigned long long int sizebin = nbin * sizeof(unsigned long long int);

	h_x = (double *)malloc(sizef);
	h_y = (double *)malloc(sizef);
	h_z = (double *)malloc(sizef);
	h_g2 = (unsigned long long int *)malloc(sizebin);

	memset(h_g2, 0, sizebin);

	nvtxRangePush("Read_File");
	double ax[numatm], ay[numatm], az[numatm];
	for (int i = 0; i < nconf; i++)
	{
		dcdreadframe(ax, ay, az, infile, numatm, xbox, ybox, zbox);
		for (int j = 0; j < numatm; j++)
		{
			h_x[i * numatm + j] = ax[j];
			h_y[i * numatm + j] = ay[j];
			h_z[i * numatm + j] = az[j];
		}
	}
	nvtxRangePop();
	cout << "Reading of input file is completed" << endl;

#pragma acc data copy(h_g2 [0:nbin]) copyin(h_x [0:nconf * numatm], h_z [0:nconf * numatm], h_y [0:nconf * numatm])

	{
		nvtxRangePush("Pair_Calculation");
		pair_gpu(h_x, h_y, h_z, h_g2, numatm, nconf, xbox, ybox, zbox, nbin);
		nvtxRangePop();
	}

	double pi = acos(-1.0l);
	double rho = (numatm) / (xbox * ybox * zbox);
	double norm = (4.0l * pi * rho) / 3.0l;
	double rl, ru, nideal;
	double g2[nbin];
	double r, gr, lngr, lngrbond, s2 = 0.0l, s2bond = 0.0l;
	double box = min(xbox, ybox);
	box = min(box, zbox);
	double del = box / (2.0l * nbin);
	nvtxRangePush("Entropy_Calculation");
	for (int i = 0; i < nbin; i++)
	{
		rl = (i)*del;
		ru = rl + del;
		nideal = norm * (ru * ru * ru - rl * rl * rl);
		g2[i] = (double)h_g2[i] / ((double)nconf * (double)numatm * nideal);
		r = (i)*del;
		pairfile << (i + 0.5l) * del << " " << g2[i] << endl;
		if (r < 2.0l)
		{
			gr = 0.0l;
		}
		else
		{
			gr = g2[i];
		}
		if (gr < 1e-5)
		{
			lngr = 0.0l;
		}
		else
		{
			lngr = log(gr);
		}

		if (g2[i] < 1e-6)
		{
			lngrbond = 0.0l;
		}
		else
		{
			lngrbond = log(g2[i]);
		}
		s2 = s2 - 2.0l * pi * rho * ((gr * lngr) - gr + 1.0l) * del * r * r;
		s2bond = s2bond - 2.0l * pi * rho * ((g2[i] * lngrbond) - g2[i] + 1.0l) * del * r * r;
	}
	nvtxRangePop();
	stwo << "s2 value is " << s2 << endl;
	stwo << "s2bond value is " << s2bond << endl;

	cout << "#Freeing Host memory" << endl;
	free(h_x);
	free(h_y);
	free(h_z);
	free(h_g2);

	cout << "#Number of atoms processed: " << numatm << endl
		 << endl;
	cout << "#Number of confs processed: " << nconf << endl
		 << endl;
	return 0;
}
#pragma acc routine seq
int round(float num)
{
	return num < 0 ? num - 0.5 : num + 0.5;
}
void pair_gpu(const double *d_x, const double *d_y, const double *d_z,
			  unsigned long long int *d_g2, int numatm, int nconf,
			  const double xbox, const double ybox, const double zbox, int d_bin)
{
	double r, cut, dx, dy, dz;
	int ig2;
	double box;
	//int myround;
	box = min(xbox, ybox);
	box = min(box, zbox);

	double del = box / (2.0 * d_bin);
	cut = box * 0.5;

	printf("\n %d %d ", nconf, numatm);
	for (int frame = 0; frame < nconf; frame++)
	{
		printf("\n %d  ", frame);
#pragma acc parallel loop collapse(2) default(present)
		for (int id1 = 0; id1 < numatm; id1++)
		{
			for (int id2 = 0; id2 < numatm; id2++)
			{
				dx = d_x[frame * numatm + id1] - d_x[frame * numatm + id2];
				dy = d_y[frame * numatm + id1] - d_y[frame * numatm + id2];
				dz = d_z[frame * numatm + id1] - d_z[frame * numatm + id2];

				dx = dx - xbox * (round(dx / xbox));
				dy = dy - ybox * (round(dy / ybox));
				dz = dz - zbox * (round(dz / zbox));

				r = sqrtf(dx * dx + dy * dy + dz * dz);
				if (r < cut)
				{
					ig2 = (int)(r / del);
#pragma acc atomic
					d_g2[ig2] = d_g2[ig2] + 1;
				}
			}
		}
	}
}