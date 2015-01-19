--ワーム・オペラ
function c28465301.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FLIP+EFFECT_TYPE_SINGLE)
	e1:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DEFCHANGE)
	e1:SetOperation(c28465301.valop)
	c:RegisterEffect(e1)
end
function c28465301.filter(c)
	return c:IsFaceup() and not (c:IsSetCard(0x3e) and c:IsRace(RACE_REPTILE))
end
function c28465301.valop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c28465301.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(-500)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end
