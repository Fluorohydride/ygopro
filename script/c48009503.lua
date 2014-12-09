--H－C ガーンデーヴァ
function c48009503.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_WARRIOR),4,2)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(48009503,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c48009503.cost)
	e1:SetTarget(c48009503.target)
	e1:SetOperation(c48009503.operation)
	c:RegisterEffect(e1)
end
function c48009503.filter(c,e,tp)
	return c:IsFaceup() and c:IsControler(1-tp) and c:IsLevelBelow(4) and (not e or c:IsRelateToEffect(e))
end
function c48009503.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c48009503.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c48009503.filter,1,nil,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,eg:GetCount(),0,0)
end
function c48009503.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c48009503.filter,nil,e,tp)
	Duel.Destroy(g,REASON_EFFECT)
end
