--ジェスター・ロード
function c72992744.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c72992744.atkcon)
	e1:SetValue(c72992744.atkval)
	c:RegisterEffect(e1)
end
function c72992744.atkcon(e)
	return not Duel.IsExistingMatchingCard(nil,0,LOCATION_MZONE,LOCATION_MZONE,1,e:GetHandler())
end
function c72992744.filter(c)
	return c:GetSequence()<5
end
function c72992744.atkval(e,c)
	return Duel.GetMatchingGroupCount(c72992744.filter,0,LOCATION_SZONE,LOCATION_SZONE,nil)*1000
end
