--D・ビデオン
function c84592800.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c84592800.cona)
	e1:SetValue(c84592800.val)
	c:RegisterEffect(e1)
	--def
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetCondition(c84592800.cond)
	c:RegisterEffect(e2)
end
function c84592800.cona(e)
	return e:GetHandler():IsAttackPos()
end
function c84592800.cond(e)
	return e:GetHandler():IsDefencePos()
end
function c84592800.val(e,c)
	return c:GetEquipCount()*800
end
