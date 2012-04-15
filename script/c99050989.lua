--ドリラゴ
function c99050989.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c99050989.dircon)
	c:RegisterEffect(e1)
end
function c99050989.filter(c)
	return c:GetAttack()<1600 or c:IsFacedown()
end
function c99050989.dircon(e)
	local tp=e:GetHandlerPlayer()
	return Duel.GetFieldGroupCount(tp,0,LOCATION_SZONE)==0 
		and not Duel.IsExistingMatchingCard(c99050989.filter,tp,0,LOCATION_MZONE,1,nil)
end
