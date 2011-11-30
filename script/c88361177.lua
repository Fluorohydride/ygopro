--ドラグニティ－アングス
function c88361177.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetCondition(c88361177.pcon)
	c:RegisterEffect(e1)
end
function c88361177.pcon(e)
	return e:GetHandler():GetEquipGroup():IsExists(Card.IsSetCard,1,nil,0x29)
end
