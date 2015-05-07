--sophiaの影霊衣
function c21105106.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c21105106.splimit)
	c:RegisterEffect(e1)
	--cannot spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c21105106.discon)
	e2:SetCost(c21105106.discost)
	e2:SetOperation(c21105106.disop)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetCondition(c21105106.rmcon)
	e3:SetCost(c21105106.rmcost)
	e3:SetTarget(c21105106.rmtg)
	e3:SetOperation(c21105106.rmop)
	c:RegisterEffect(e3)
end
function c21105106.splimit(e,se,sp,st)
	return e:GetHandler():IsLocation(LOCATION_HAND) and bit.band(st,SUMMON_TYPE_RITUAL)==SUMMON_TYPE_RITUAL
end
function c21105106.mat_filter(c)
	return false
end
function c21105106.discon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c21105106.cfilter(c)
	return c:IsSetCard(0xb4) and c:IsType(TYPE_SPELL) and c:IsDiscardable()
end
function c21105106.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsDiscardable()
		and Duel.IsExistingMatchingCard(c21105106.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local g=Duel.SelectMatchingCard(tp,c21105106.cfilter,tp,LOCATION_HAND,0,1,1,nil)
	g:AddCard(e:GetHandler())
	Duel.SendtoGrave(g,REASON_DISCARD+REASON_COST)
end
function c21105106.disop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_MAIN1)
	e1:SetTargetRange(0,1)
	e1:SetTarget(c21105106.sumlimit)
	Duel.RegisterEffect(e1,tp)
end
function c21105106.sumlimit(e,c,sump,sumtype,sumpos,targetp,se)
	return c:IsLocation(LOCATION_EXTRA)
end
function c21105106.rmcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_RITUAL
end
function c21105106.rmcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetActivityCount(tp,ACTIVITY_NORMALSUMMON)==0
		and Duel.GetActivityCount(tp,ACTIVITY_SPSUMMON)==1 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	Duel.RegisterEffect(e2,tp)
	local e3=e1:Clone()
	e3:SetCode(EFFECT_CANNOT_MSET)
	Duel.RegisterEffect(e3,tp)
end
function c21105106.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToRemove,tp,LOCATION_ONFIELD+LOCATION_GRAVE,LOCATION_ONFIELD+LOCATION_GRAVE,1,e:GetHandler()) end
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,LOCATION_ONFIELD+LOCATION_GRAVE,LOCATION_ONFIELD+LOCATION_GRAVE,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c21105106.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,LOCATION_ONFIELD+LOCATION_GRAVE,LOCATION_ONFIELD+LOCATION_GRAVE,e:GetHandler())
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
function c21105106.filter(c,fc,tp)
	return c:IsControler(tp) and c:IsLocation(LOCATION_MZONE) and c:GetRitualLevel(fc)==c:GetLevel()
end
function c21105106.ritual_custom_condition(c,mg)
	local tp=c:GetControler()
	local g=mg:Filter(c21105106.filter,c,c,tp)
	return g:IsExists(c21105106.ritfilter1,1,nil,c:GetLevel(),g)
end
function c21105106.ritfilter1(c,lv,mg)
	lv=lv-c:GetLevel()
	if lv<2 then return false end
	local mg2=mg:Clone()
	mg2:Remove(Card.IsRace,nil,c:GetRace())
	return mg2:IsExists(c21105106.ritfilter2,1,nil,lv,mg2)
end
function c21105106.ritfilter2(c,lv,mg)
	local clv=c:GetLevel()
	lv=lv-clv
	if lv<1 then return false end
	local mg2=mg:Clone()
	mg2:Remove(Card.IsRace,nil,c:GetRace())
	return mg2:IsExists(c21105106.ritfilter3,1,nil,lv)
end
function c21105106.ritfilter3(c,lv)
	return c:GetLevel()==lv
end
function c21105106.ritual_custom_operation(c,mg)
	local tp=c:GetControler()
	local lv=c:GetLevel()
	local g=mg:Filter(c21105106.filter,c,c,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g1=g:FilterSelect(tp,c21105106.ritfilter1,1,1,nil,lv,g)
	local tc1=g1:GetFirst()
	lv=lv-tc1:GetLevel()
	g:Remove(Card.IsRace,nil,tc1:GetRace())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g2=g:FilterSelect(tp,c21105106.ritfilter2,1,1,nil,lv,g)
	local tc2=g2:GetFirst()
	lv=lv-tc2:GetLevel()
	g:Remove(Card.IsRace,nil,tc2:GetRace())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g3=g:FilterSelect(tp,c21105106.ritfilter3,1,1,nil,lv)
	g1:Merge(g2)
	g1:Merge(g3)
	c:SetMaterial(g1)
end
