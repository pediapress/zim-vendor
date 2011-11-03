/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef cxxtools_xmlrpc_RemoteProcedure_h
#define cxxtools_xmlrpc_RemoteProcedure_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/xmlrpc/client.h>
#include <cxxtools/xmlrpc/fault.h>
#include <cxxtools/xmlrpc/result.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/serializer.h>
#include <cxxtools/signal.h>
#include <string>

namespace cxxtools {

namespace xmlrpc {

class Fault;

class CXXTOOLS_XMLRPC_API IRemoteProcedure
{
    friend class ClientImpl;

    public:
        IRemoteProcedure(Client& client, const String& name)
        : _client(&client)
        , _name(name)
        { }

        IRemoteProcedure(Client& client, const std::string& name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        IRemoteProcedure(Client& client, const char* name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        virtual ~IRemoteProcedure()
        { cancel(); }

        Client& client()
        { return *_client; }

        const String& name() const
        { return _name; }

        virtual void setFault(int rc, const std::string& msg) = 0;

        virtual bool failed() const = 0;

        void cancel()
        {
            if (_client && _client->activeProcedure() == this)
                _client->cancel();
        }

    protected:
        virtual void onFinished() = 0;

    private:
        Client* _client;
        String _name;
};


template <typename R>
class RemoteProcedureBase : public IRemoteProcedure
{
    public:
        RemoteProcedureBase(Client& client, const std::string& name)
        : IRemoteProcedure(client, name),
          _result(client)
        { }

        void setFault(int rc, const std::string& msg)
        {
            _result.setFault(rc, msg);
        }

        const R& result()
        {
            return _result.get();
        }

        virtual bool failed() const
        {
            return _result.failed();
        }

        Signal< const Result<R> & > finished;

    protected:
        void onFinished()
        { finished.send(_result); }

        Result<R> _result;
        Deserializer<R> _r;
};


// this part is generated with remoteprocedure.pl:

template <typename R,
          typename A1 = cxxtools::Void,
          typename A2 = cxxtools::Void,
          typename A3 = cxxtools::Void,
          typename A4 = cxxtools::Void,
          typename A5 = cxxtools::Void,
          typename A6 = cxxtools::Void,
          typename A7 = cxxtools::Void,
          typename A8 = cxxtools::Void,
          typename A9 = cxxtools::Void,
          typename A10 = cxxtools::Void>
class RemoteProcedure : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            _a10.begin(a10);

            this->_r.begin(this->_result.value());

            ISerializer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().beginCall(this->_r, *this, argv, 10);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            _a10.begin(a10);
            this->_r.begin(this->_result.value());

            ISerializer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().call(this->_r, *this, argv, 10);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
        Serializer<A6> _a6;
        Serializer<A7> _a7;
        Serializer<A8> _a8;
        Serializer<A9> _a9;
        Serializer<A10> _a10;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8,
          typename A9>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);

            this->_r.begin(this->_result.value());

            ISerializer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().beginCall(this->_r, *this, argv, 9);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            this->_r.begin(this->_result.value());

            ISerializer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().call(this->_r, *this, argv, 9);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
        Serializer<A6> _a6;
        Serializer<A7> _a7;
        Serializer<A8> _a8;
        Serializer<A9> _a9;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);

            this->_r.begin(this->_result.value());

            ISerializer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().beginCall(this->_r, *this, argv, 8);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            this->_r.begin(this->_result.value());

            ISerializer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().call(this->_r, *this, argv, 8);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
        Serializer<A6> _a6;
        Serializer<A7> _a7;
        Serializer<A8> _a8;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);

            this->_r.begin(this->_result.value());

            ISerializer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().beginCall(this->_r, *this, argv, 7);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            this->_r.begin(this->_result.value());

            ISerializer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().call(this->_r, *this, argv, 7);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
        Serializer<A6> _a6;
        Serializer<A7> _a7;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);

            this->_r.begin(this->_result.value());

            ISerializer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().beginCall(this->_r, *this, argv, 6);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            this->_r.begin(this->_result.value());

            ISerializer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().call(this->_r, *this, argv, 6);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            return this->call(a1, a2, a3, a4, a5, a6);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
        Serializer<A6> _a6;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5>
class RemoteProcedure<R, A1, A2, A3, A4, A5,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);

            this->_r.begin(this->_result.value());

            ISerializer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().beginCall(this->_r, *this, argv, 5);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            this->_r.begin(this->_result.value());

            ISerializer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().call(this->_r, *this, argv, 5);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            return this->call(a1, a2, a3, a4, a5);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
        Serializer<A5> _a5;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4>
class RemoteProcedure<R, A1, A2, A3, A4,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);

            this->_r.begin(this->_result.value());

            ISerializer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().beginCall(this->_r, *this, argv, 4);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            this->_r.begin(this->_result.value());

            ISerializer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().call(this->_r, *this, argv, 4);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            return this->call(a1, a2, a3, a4);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
        Serializer<A4> _a4;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3>
class RemoteProcedure<R, A1, A2, A3,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);

            this->_r.begin(this->_result.value());

            ISerializer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().beginCall(this->_r, *this, argv, 3);
        }

        const R& call(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            this->_r.begin(this->_result.value());

            ISerializer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().call(this->_r, *this, argv, 3);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2, const A3& a3)
        {
            return this->call(a1, a2, a3);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
        Serializer<A3> _a3;
};


template <typename R,
          typename A1,
          typename A2>
class RemoteProcedure<R, A1, A2,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);

            this->_r.begin(this->_result.value());

            ISerializer* argv[2] = { &_a1, &_a2 };
            this->client().beginCall(this->_r, *this, argv, 2);
        }

        const R& call(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            this->_r.begin(this->_result.value());

            ISerializer* argv[2] = { &_a1, &_a2 };
            this->client().call(this->_r, *this, argv, 2);
            return this->_result.get();
        }

        const R& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

    private:
        Serializer<A1> _a1;
        Serializer<A2> _a2;
};


template <typename R,
          typename A1>
class RemoteProcedure<R, A1,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        void begin(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1);

            this->_r.begin(this->_result.value());

            ISerializer* argv[1] = { &_a1 };
            this->client().beginCall(this->_r, *this, argv, 1);
        }

        const R& call(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            this->_r.begin(this->_result.value());

            ISerializer* argv[1] = { &_a1 };
            this->client().call(this->_r, *this, argv, 1);
            return this->_result.get();
        }

        const R& operator()(const A1& a1)
        {
            return this->call(a1);
        }

    private:
        Serializer<A1> _a1;
};


template <typename R>
class RemoteProcedure<R,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
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

// end of generation

}

}

#endif
