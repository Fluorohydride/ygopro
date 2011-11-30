--深黒の落とし穴
function c28654932.initial_effect(c)
	--Activate(summon)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetTarget(c28654932.target)
	e1:SetOperation(c28654932.activate)
	c:RegisterEffect(e1)
end
function c28654932.filter(c,e)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT) and c:GetLevel()>=5
		and (not e or c:IsRelateToEffect(e)) and c:IsAbleToRemove()
end
function c28654932.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c28654932.filter,1,nil,nil) end
	local g=eg:Filter(c28654932.filter,nil,nil)
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c28654932.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c28654932.filter,nil,e)
	if g:GetCount()>0 then
		Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	end
end
