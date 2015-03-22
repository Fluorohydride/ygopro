--スパイダー・ウェブ
function c69408987.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--to defence
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_DAMAGE_STEP_END)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c69408987.poscon)
	e2:SetOperation(c69408987.posop)
	c:RegisterEffect(e2)
end
function c69408987.poscon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	return a:IsPosition(POS_FACEUP_ATTACK) and a:IsRelateToBattle()
end
function c69408987.posop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	if Duel.ChangePosition(a,POS_FACEUP_DEFENCE)~=0 then
		e:GetHandler():CreateRelation(a,RESET_EVENT+0x1fe0000)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,3)
		e1:SetCondition(c69408987.poscon2)
		a:RegisterEffect(e1)
	end
end
function c69408987.poscon2(e)
	return e:GetOwner():IsRelateToCard(e:GetHandler())
end
