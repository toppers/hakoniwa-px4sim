from abc import ABC, abstractmethod

class IHakoAreaPropAccessor(ABC):
    @abstractmethod
    def get_property(self, area_id: str):
        """
        指定されたエリアIDに関連するプロパティを取得するメソッド。
        
        Args:
            area_id (str): 対象のエリアのID
        
        Returns:
            HakoAreaProperty: エリアに関連するプロパティ情報
            None: 該当するエリアIDが存在しない場合
        """
        pass
