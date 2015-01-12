--ヴェルズ・ナイトメア
function c359563.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsAttribute,ATTRIBUTE_DARK),4,2)
	c:EnableReviveLimit()
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(359563,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c359563.cost)
	e1:SetTarget(c359563.target)
	e1:SetOperation(c359563.operation)
	c:RegisterEffect(e1)
end
function c359563.filter(c,e,tp)
	return c:IsFaceup() and c:GetSummonPlayer()==1-tp and (not e or c:IsRelateToEffect(e))
end
function c359563.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c359563.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c359563.filter,1,nil,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c359563.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c359563.filter,nil,e,tp)
	Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
end
