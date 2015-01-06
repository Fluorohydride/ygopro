--ライフ・コーディネイター
function c76214441.initial_effect(c)
	--Negate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(76214441,0))
	e1:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c76214441.discon)
	e1:SetCost(c76214441.discost)
	e1:SetTarget(c76214441.distg)
	e1:SetOperation(c76214441.disop)
	c:RegisterEffect(e1)
end
function c76214441.discon(e,tp,eg,ep,ev,re,r,rp)
	if ep==tp or (re:GetHandler():IsType(TYPE_SPELL+TYPE_TRAP) and not re:IsHasType(EFFECT_TYPE_ACTIVATE)) or not Duel.IsChainNegatable(ev) then return false end
	local ex,cg,ct,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_DAMAGE)
	if ex then return true end
	ex,cg,ct,cp,cv=Duel.GetOperationInfo(ev,CATEGORY_RECOVER)
	return ex and ((cp~=PLAYER_ALL and Duel.IsPlayerAffectedByEffect(cp,EFFECT_REVERSE_RECOVER)) or
		(cp==PLAYER_ALL and (Duel.IsPlayerAffectedByEffect(0,EFFECT_REVERSE_RECOVER) or Duel.IsPlayerAffectedByEffect(1,EFFECT_REVERSE_RECOVER))))
end
function c76214441.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsDiscardable() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST+REASON_DISCARD)
end
function c76214441.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsDestructable() and re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c76214441.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
