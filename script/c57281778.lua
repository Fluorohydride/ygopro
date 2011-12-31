--龍骨鬼
function c57281778.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(57281778,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetCondition(c57281778.descon)
	e1:SetTarget(c57281778.destg)
	e1:SetOperation(c57281778.desop)
	c:RegisterEffect(e1)
end
function c57281778.descon(e,tp,eg,ep,ev,re,r,rp)
	local t=Duel.GetAttackTarget()
	if ev==1 then t=Duel.GetAttacker() end
	e:SetLabelObject(t)
	return t and t:IsRace(RACE_SPELLCASTER+RACE_WARRIOR) and t:IsRelateToBattle()
end
function c57281778.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetLabelObject():IsDestructable() end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetLabelObject(),1,0,0)
end
function c57281778.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:IsRelateToBattle() then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
