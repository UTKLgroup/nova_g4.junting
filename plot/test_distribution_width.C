
double getDistributionWidth(std::vector<double> &vec, double width){
  double quantileBeg = (1. - width) / 2.;
  double quantileEnd = quantileBeg + width;

  int indexBeg = (int)(vec.size() * quantileBeg);
  nth_element(vec.begin(), vec.begin() + indexBeg, vec.end());
  double beg = vec[indexBeg];

  int indexEnd = (int)(vec.size() * quantileEnd);
  std::nth_element(vec.begin(), vec.begin() + indexEnd, vec.end());
  double end = vec[indexEnd];

  return end - beg;
}

void test_distribution_width()
{
  vector<double> numbers;
  for (int i = 0; i < 10; i++)
    numbers.push_back((double) i);
  for (double number : numbers)
    cout << number << endl;

  for (double i = 0.1; i < 1; i +=0.05)
    cout << "getDistributionWidth(numbers," << i  << ") = " << getDistributionWidth(numbers, i) << endl;
}
