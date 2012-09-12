--強烈なはたき落とし
function c4440873.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_HANDES)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCondition(c4440873.condition)
	e1:SetTarget(c4440873.target)
	e1:SetOperation(c4440873.activate)
	c:RegisterEffect(e1)
end
function c4440873.cfilter(c,tp)
	return c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK)
end
function c4440873.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c4440873.cfilter,1,nil,1-tp)
end
function c4440873.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,1)
end
function c4440873.filter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK)
end
function c4440873.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=eg:Filter(c4440873.filter,nil,e,1-tp)
	if sg:GetCount()==0 then
	elseif sg:GetCount()==1 then
		Duel.SendtoGrave(sg,REASON_EFFECT+REASON_DISCARD)
	else
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_DISCARD)
		local dg=sg:Select(1-tp,1,1,nil)
		Duel.SendtoGrave(dg,REASON_EFFECT+REASON_DISCARD)
	end
end
