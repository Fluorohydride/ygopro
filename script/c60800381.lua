--ジャンク・ウォリアー
function c60800381.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,c60800381.tfilter,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60800381,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c60800381.con)
	e1:SetOperation(c60800381.op)
	c:RegisterEffect(e1)
end
function c60800381.tfilter(c)
	return c:IsCode(63977008) or c:IsHasEffect(20932152)
end
function c60800381.con(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SYNCHRO
end
function c60800381.filter(c)
	return c:IsFaceup() and c:IsLevelBelow(2)
end
function c60800381.op(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetMatchingGroup(c60800381.filter,tp,LOCATION_MZONE,0,nil)
	if g:GetCount()>0 then
		local atk=0
		local tc=g:GetFirst()
		while tc do
			atk=atk+tc:GetAttack()
			tc=g:GetNext()
		end
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
