--ミスティック・ソードマン LV6
function c60482781.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(60482781,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetCondition(c60482781.descon)
	e1:SetTarget(c60482781.destg)
	e1:SetOperation(c60482781.desop)
	c:RegisterEffect(e1)
	--summon limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_LIMIT_SUMMON_PROC)
	e2:SetCondition(c60482781.sumcon)
	c:RegisterEffect(e2) 
end
c60482781.lvdncount=2
c60482781.lvdn={47507260,74591968}
function c60482781.descon(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	return e:GetHandler()==Duel.GetAttacker() and d and d:IsFacedown() and d:IsDefencePos()
end
function c60482781.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttackTarget(),1,0,0)
end
function c60482781.desop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d:IsRelateToBattle() then
		if d:GetOwner()~=tp and Duel.SelectYesNo(tp,aux.Stringid(60482781,1)) then
			Duel.Destroy(d,REASON_EFFECT,LOCATION_DECK)
		else
			Duel.Destroy(d,REASON_EFFECT)
		end
	end
end
function c60482781.sumcon(e,c)
	if not c then return true end
	return false
end
