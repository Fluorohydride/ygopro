--ジュラック・ヴェルヒプト
function c65961683.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(Card.IsRace,RACE_DINOSAUR),1)
	c:EnableReviveLimit()
	--Atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetOperation(c65961683.valop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(65961683,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BATTLE_START)
	e2:SetCondition(c65961683.descon)
	e2:SetTarget(c65961683.destg)
	e2:SetOperation(c65961683.desop)
	c:RegisterEffect(e2)
end
function c65961683.valop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetSummonType()~=SUMMON_TYPE_SYNCHRO then return end
	local g=c:GetMaterial()
	local tc=g:GetFirst()
	local atk=0
	while tc do
		local tatk=tc:GetTextAttack()
		if tatk<0 then tatk=0 end
		atk=atk+tatk
		tc=g:GetNext()
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetValue(atk)
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_SET_DEFENCE)
	c:RegisterEffect(e2)
end
function c65961683.descon(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	return e:GetHandler()==Duel.GetAttacker() and d and d:IsPosition(POS_FACEDOWN_DEFENCE)
end
function c65961683.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetAttackTarget():IsDestructable() end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttackTarget(),1,0,0)
end
function c65961683.desop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d:IsRelateToBattle() then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
