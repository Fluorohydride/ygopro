--エヴォルカイザー・ソルデ
function c18511599.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,c18511599.xyzfilter,2)
	c:EnableReviveLimit()
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c18511599.indcon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(18511599,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c18511599.cost)
	e2:SetTarget(c18511599.target)
	e2:SetOperation(c18511599.operation)
	c:RegisterEffect(e2)
end
function c18511599.indcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetOverlayCount()~=0
end
function c18511599.xyzfilter(c)
	return c:GetLevel()==6 and c:IsRace(RACE_DINOSAUR)
end
function c18511599.filter(c,e,tp)
	return c:IsControler(1-tp) and (not e or c:IsRelateToEffect(e))
end
function c18511599.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c18511599.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c18511599.filter,1,nil,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,eg:GetCount(),0,0)
end
function c18511599.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c18511599.filter,nil,e,tp)
	Duel.Destroy(g,REASON_EFFECT)
end