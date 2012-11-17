--出目出し
function c31863912.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_SPSUMMON)
	e1:SetTarget(c31863912.target1)
	e1:SetOperation(c31863912.operation)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(31863912,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c31863912.target2)
	e2:SetOperation(c31863912.operation)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
end
function c31863912.cfilter(c,sp)
	return c:IsFaceup() and c:GetSummonPlayer()==sp
end
function c31863912.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS,true)
	if res and teg:IsExists(c31863912.cfilter,1,nil,1-tp) then
		e:SetLabel(1)
		Duel.SetTargetCard(teg)
		Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
	else
		e:SetLabel(0)
	end
end
function c31863912.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c31863912.cfilter,1,nil,1-tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
end
function c31863912.filter(c,sp,e,lv)
	return c:IsFaceup() and c:GetSummonPlayer()==sp and c:GetLevel()==lv and c:IsAbleToHand() and c:IsRelateToEffect(e)
end
function c31863912.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local dc=Duel.TossDice(tp,1)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c31863912.filter,nil,1-tp,e,dc)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
	end
end
