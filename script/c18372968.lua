--カミソーリトカゲ
function c18372968.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(18372968,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetCondition(c18372968.descon)
	e1:SetTarget(c18372968.destg)
	e1:SetOperation(c18372968.desop)
	c:RegisterEffect(e1)
end
function c18372968.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_REPTILE)
end
function c18372968.descon(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	return e:GetHandler()==Duel.GetAttacker() and d and d:IsPosition(POS_FACEDOWN_DEFENCE)
		and Duel.IsExistingMatchingCard(c18372968.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c18372968.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttackTarget(),1,0,0)
end
function c18372968.desop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d:IsRelateToBattle() and d:IsPosition(POS_FACEDOWN_DEFENCE)
		and Duel.IsExistingMatchingCard(c18372968.cfilter,tp,LOCATION_MZONE,0,1,e:GetHandler()) then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
