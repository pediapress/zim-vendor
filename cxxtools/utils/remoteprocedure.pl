#!/usr/bin/perl -w

=head1 NAME

remoteprocedure.pl

=head1 SYNOPSIS

remoteprocedure.pl -n 8 -N Pt

=head1 DESCRIPTION

Generates template class RemoteProcedure with variable number of arguments.

=head1 OPTIONS

-n <number>
    maximum number of arguments (default 5)

-N <string>
    namespace (default cxxtools)

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use Getopt::Std;

my %opt;

getopts('n:N:', \%opt);

my $N = $opt{n} || 5;
my $ns = $opt{N} || 'cxxtools';

sub constRef {
  my $n = shift;
  join ', ', map { "const A$_& a$_" } (1..$n);
}

sub argv {
  my $n = shift;
  join ', ', map { "&_a$_" } (1..$n);
}

sub params {
  my $n = shift;
  join ', ', map { "a$_" } (1..$n);
}

sub printBegin {
  my $N = shift;
  for (my $n = 1; $n <= $N; ++$n)
  {
print <<EOF;
            _a$n.begin(a$n);
EOF
  }
}

########################################################################
## first
##
print <<EOF;
template <typename R,
EOF

my $argv = argv($N);
my $constRef = constRef($N);
my $params = params($N);

for (my $n = 1; $n < $N; ++$n)
{
  print <<EOF;
          typename A$n = ${ns}::Void,
EOF
}
  print <<EOF;
          typename A$N = ${ns}::Void>
class RemoteProcedure : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin($constRef)
        {
            this->_result.clearFault();

EOF
  printBegin($N);

  print <<EOF;

            this->_r.begin(this->_result.value());

            ISerializer* argv[$N] = { $argv };
            this->client().beginCall(this->_r, *this, argv, $N);
        }

        const R& call($constRef)
        {
            this->_result.clearFault();

EOF
  printBegin($N);

  print <<EOF;
            this->_r.begin(this->_result.value());

            ISerializer* argv[$N] = { $argv };
            this->client().call(this->_r, *this, argv, $N);
            return this->_result.get();
        }

        const R& operator()($constRef)
        {
            return this->call($params);
        }

    private:
EOF

for (my $n = 1; $n <= $N; ++$n)
{
  print <<EOF;
        Serializer<A$n> _a$n;
EOF
}
  print <<EOF;
};

EOF

########################################################################
## loop
##
  for (my $nn = $N - 1; $nn >= 1; --$nn)
  {
    my $argv = argv($nn);
    my $constRef = constRef($nn);
    my $params = params($nn);
    print <<EOF;

template <typename R,
EOF

    for (my $n = 1; $n < $nn; ++$n)
    {
  print <<EOF;
          typename A$n,
EOF
    }

  print <<EOF;
          typename A$nn>
EOF

  my $tparams = join ', ', map { "A$_" } (1..$nn);

  print <<EOF;
class RemoteProcedure<R, $tparams,
EOF
  for (my $n = $nn; $n < $N - 1; ++$n)
  {
    print <<EOF;
                      ${ns}::Void,
EOF
  }
  print <<EOF;
                      ${ns}::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin($constRef)
        {
            this->_result.clearFault();

EOF
  printBegin($nn);

  print <<EOF;

            this->_r.begin(this->_result.value());

            ISerializer* argv[$nn] = { $argv };
            this->client().beginCall(this->_r, *this, argv, $nn);
        }

        const R& call($constRef)
        {
            this->_result.clearFault();

EOF
  printBegin($nn);

  print <<EOF;
            this->_r.begin(this->_result.value());

            ISerializer* argv[$nn] = { $argv };
            this->client().call(this->_r, *this, argv, $nn);
            return this->_result.get();
        }

        const R& operator()($constRef)
        {
            return this->call($params);
        }

    private:
EOF

for (my $n = 1; $n <= $nn; ++$n)
{
  print <<EOF;
        Serializer<A$n> _a$n;
EOF
}
  print <<EOF;
};

EOF
  }

########################################################################
## last
##
    print <<EOF;

template <typename R>
class RemoteProcedure<R,
EOF
  for (my $n = 0; $n < $N - 1; ++$n)
  {
    print <<EOF;
                      ${ns}::Void,
EOF
  }
  print <<EOF;
                      ${ns}::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin()
        {
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            ISerializer* argv[1] = { 0 };
            this->client().beginCall(this->_r, *this, argv, 0);
        }

        const R& call()
        {
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            ISerializer* argv[1] = { 0 };
            this->client().call(this->_r, *this, argv, 0);
            return this->_result.get();
        }

        const R& operator()()
        {
            return this->call();
        }
};

EOF
