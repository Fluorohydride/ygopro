--ファイナル・インゼクション
function c51549976.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c51549976.cost)
	e1:SetTarget(c51549976.target)
	e1:SetOperation(c51549976.activate)
	c:RegisterEffect(e1)
end
function c51549976.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x56) and c:IsAbleToGraveAsCost()
end
function c51549976.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c51549976.cfilter,tp,LOCATION_ONFIELD,0,5,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c51549976.cfilter,tp,LOCATION_ONFIELD,0,5,5,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c51549976.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c51549976.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_ONFIELD,nil)
	Duel.Destroy(g,REASON_EFFECT)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetTargetRange(0,1)
	e1:SetCondition(c51549976.actcon)
	e1:SetValue(c51549976.aclimit)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c51549976.actcon(e)
	local ph=Duel.GetCurrentPhase()
	return ph>=PHASE_BATTLE and ph<=PHASE_DAMAGE_CAL
end
function c51549976.aclimit(e,re,tp)
	return re:GetHandler():IsType(TYPE_MONSTER) and re:GetHandler():IsLocation(LOCATION_HAND+LOCATION_GRAVE)
end
