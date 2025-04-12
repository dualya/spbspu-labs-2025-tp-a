#include "dataStruct.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace duhanina
{
  Iofmtguard::Iofmtguard(std::basic_ios< char >& s):
    s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
  {}

  Iofmtguard::~Iofmtguard()
  {
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
  }

  std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    char c = '0';
    in >> c;
    if (in && c != dest.exp)
    {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, LongLongIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    in >> dest.ref >> DelimiterIO{'l'} >> DelimiterIO{'l'};
    return in;
  }

  std::istream& operator>>(std::istream& in, ComplexIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    double real = 0.0;
    double imag = 0.0;
    in >> DelimiterIO{ '#' };
    in >> DelimiterIO{ 'c' };
    in >> DelimiterIO{ '(' };
    in >> real;
    in >> imag;
    in >> DelimiterIO{ ')' };
    if (in)
    {
      dest.ref = std::complex< double >(real, imag);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, StringIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    in >> DelimiterIO{'"'};
    std::getline(in, dest.ref, '"');
    return in;
  }

  std::istream& operator>>(std::istream& in, LabelIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    std::string label;
    if ((in >> label) && label != dest.exp)
    {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, DataStruct& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    DataStruct temp;
    in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' };
    bool k1 = false;
    bool k2 = false;
    bool k3 = false;
    std::string label;

//    for (size_t i = 0; i < 3; i++)
    while (in >> label)
    {
    //  if (!(in >> StringIO{ label }))
    //  {
    //    break;
    //  }
      if (label == "key1")
      {
        in >> LongLongIO{ temp.key1 };
        k1 = true;
      }
      else if (label == "key2")
      {
        in >> ComplexIO{ temp.key2 };
        k2 = true;
      }
      else if (label == "key3")
      {
        in >> StringIO{ temp.key3 };
        k3 = true;
      }
      in >> DelimiterIO{ ':' };
    }
    in >> DelimiterIO{ ')' };
    if (k1 && k2 && k3)
    {
      dest = temp;
    }
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const DataStruct& src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
      return out;
    }
    Iofmtguard guard(out);
    out << "(:key1 " << src.key1 << "ll:";
    out << "key2 #c(" << std::fixed << std::setprecision(1) << src.key2.real();
    out << " " << src.key2.imag() << "):";
    out << "key3 \"" << src.key3 << "\":)";
    return out;
  }

  bool DataStruct::operator<(const DataStruct& other) const
  {
    if (key1 != other.key1)
    {
      return key1 < other.key1;
    }
    const double thisMod = std::abs(key2);
    const double otherMod = std::abs(other.key2);
    if (thisMod != otherMod)
    {
      return thisMod < otherMod;
    }
    return key3.size() < other.key3.size();
  }
}
