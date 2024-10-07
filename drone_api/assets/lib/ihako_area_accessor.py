from abc import ABC, abstractmethod

from ihako_object_space import IHakoObjectSpace

class IHakoAreaAccessor(ABC):
    @abstractmethod
    def get_area_id(self, object_space: IHakoObjectSpace) -> str:
        """
        与えられた HakoObjectSpace オブジェクトに基づき、
        オブジェクトが属するエリアの area_id を返すメソッド。
        
        Args:
            object_space (HakoObjectSpace): 対象のオブジェクト空間情報
        
        Returns:
            str: オブジェクトが属するエリアの area_id
        """
        pass
