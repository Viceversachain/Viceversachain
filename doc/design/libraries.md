# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libviceversachain_cli*         | RPC client functionality used by *viceversachain-cli* executable |
| *libviceversachain_common*      | Home for common functionality shared by different executables and libraries. Similar to *libviceversachain_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libviceversachain_consensus*   | Stable, backwards-compatible consensus functionality used by *libviceversachain_node* and *libviceversachain_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libviceversachainconsensus*    | Shared library build of static *libviceversachain_consensus* library |
| *libviceversachain_kernel*      | Consensus engine and support library used for validation by *libviceversachain_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libviceversachainqt*           | GUI functionality used by *viceversachain-qt* and *viceversachain-gui* executables |
| *libviceversachain_ipc*         | IPC functionality used by *viceversachain-node*, *viceversachain-wallet*, *viceversachain-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libviceversachain_node*        | P2P and RPC server functionality used by *viceversachaind* and *viceversachain-qt* executables. |
| *libviceversachain_util*        | Home for common functionality shared by different executables and libraries. Similar to *libviceversachain_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libviceversachain_wallet*      | Wallet functionality used by *viceversachaind* and *viceversachain-wallet* executables. |
| *libviceversachain_wallet_tool* | Lower-level wallet functionality used by *viceversachain-wallet* executable. |
| *libviceversachain_zmq*         | [ZeroMQ](../zmq.md) functionality used by *viceversachaind* and *viceversachain-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libviceversachain_consensus* and *libviceversachain_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libviceversachain_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libviceversachain_node* code lives in `src/node/` in the `node::` namespace
  - *libviceversachain_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libviceversachain_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libviceversachain_util* code lives in `src/util/` in the `util::` namespace
  - *libviceversachain_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

viceversachain-cli[viceversachain-cli]-->libviceversachain_cli;

viceversachaind[viceversachaind]-->libviceversachain_node;
viceversachaind[viceversachaind]-->libviceversachain_wallet;

viceversachain-qt[viceversachain-qt]-->libviceversachain_node;
viceversachain-qt[viceversachain-qt]-->libviceversachainqt;
viceversachain-qt[viceversachain-qt]-->libviceversachain_wallet;

viceversachain-wallet[viceversachain-wallet]-->libviceversachain_wallet;
viceversachain-wallet[viceversachain-wallet]-->libviceversachain_wallet_tool;

libviceversachain_cli-->libviceversachain_util;
libviceversachain_cli-->libviceversachain_common;

libviceversachain_common-->libviceversachain_consensus;
libviceversachain_common-->libviceversachain_util;

libviceversachain_kernel-->libviceversachain_consensus;
libviceversachain_kernel-->libviceversachain_util;

libviceversachain_node-->libviceversachain_consensus;
libviceversachain_node-->libviceversachain_kernel;
libviceversachain_node-->libviceversachain_common;
libviceversachain_node-->libviceversachain_util;

libviceversachainqt-->libviceversachain_common;
libviceversachainqt-->libviceversachain_util;

libviceversachain_wallet-->libviceversachain_common;
libviceversachain_wallet-->libviceversachain_util;

libviceversachain_wallet_tool-->libviceversachain_wallet;
libviceversachain_wallet_tool-->libviceversachain_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class viceversachain-qt,viceversachaind,viceversachain-cli,viceversachain-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libviceversachain_wallet* and *libviceversachain_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libviceversachain_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libviceversachain_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libviceversachain_common* should serve a similar function as *libviceversachain_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libviceversachain_util* and *libviceversachain_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for viceversachain-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libviceversachain_kernel* is not supposed to depend on *libviceversachain_common*, only *libviceversachain_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libviceversachain_kernel* should only depend on *libviceversachain_util* and *libviceversachain_consensus*.

- The only thing that should depend on *libviceversachain_kernel* internally should be *libviceversachain_node*. GUI and wallet libraries *libviceversachainqt* and *libviceversachain_wallet* in particular should not depend on *libviceversachain_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libviceversachain_consensus*, *libviceversachain_common*, and *libviceversachain_util*, instead of *libviceversachain_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libviceversachainqt*, *libviceversachain_node*, *libviceversachain_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libviceversachain_node* to *libviceversachain_kernel* as part of [The libviceversachainkernel Project #24303](https://github.com/viceversachain/viceversachain/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/viceversachain/viceversachain/issues/15732)
