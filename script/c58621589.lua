--誘惑のシャドウ
function c58621589.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHANGE_POS)
	e1:SetTarget(c58621589.target)
	e1:SetOperation(c58621589.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_MSET)
	e2:SetTarget(c58621589.target2)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetTarget(c58621589.target2)
	c:RegisterEffect(e3)
end
function c58621589.filter1(c,tp)
	return c:IsFacedown() and c:IsPreviousPosition(POS_FACEUP) and c:IsControler(1-tp)
end
function c58621589.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c58621589.filter1,1,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c58621589.filter2(c,tp)
	return c:IsFacedown() and c:IsControler(1-tp)
end
function c58621589.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c58621589.filter2,1,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c58621589.filter3(c,e)
	return c:IsFacedown() and c:IsRelateToEffect(e)
end
function c58621589.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c58621589.filter3,nil,e,tp)
	Duel.ChangePosition(g,0x1,0x1,0x1,0x1,true)
end
