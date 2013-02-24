--アイスバーン
function c54059040.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_SUMMON+TIMING_SPSUMMON)
	e1:SetTarget(c54059040.target1)
	e1:SetOperation(c54059040.operation)
	c:RegisterEffect(e1)
	--pos
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(54059040,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c54059040.target2)
	e2:SetOperation(c54059040.operation)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c54059040.cfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER)
end
function c54059040.pfilter(c,e)
	return c:IsPosition(POS_FACEUP_ATTACK) and not c:IsAttribute(ATTRIBUTE_WATER) and (not e or c:IsRelateToEffect(e))
end
function c54059040.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SUMMON_SUCCESS,true)
	if not res then
		res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS,true)
	end
	if res and Duel.IsExistingMatchingCard(c54059040.cfilter,tp,LOCATION_MZONE,0,1,nil)
		and teg:IsExists(c54059040.pfilter,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(54059040,1)) then
		e:SetLabel(1)
		Duel.SetTargetCard(teg)
		Duel.SetOperationInfo(0,CATEGORY_POSITION,teg,teg:GetCount(),0,0)
	else
		e:SetLabel(0)
	end
end
function c54059040.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.IsExistingMatchingCard(c54059040.cfilter,tp,LOCATION_MZONE,0,1,nil)
		and eg:IsExists(c54059040.pfilter,1,nil) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c54059040.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c54059040.pfilter,nil,e)
	Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
end
