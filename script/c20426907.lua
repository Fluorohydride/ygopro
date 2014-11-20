--機殻の再星
function c20426907.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_SUMMON+TIMING_SPSUMMON)
	e1:SetTarget(c20426907.distg1)
	e1:SetOperation(c20426907.disop)
	c:RegisterEffect(e1)
	--disable
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DISABLE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c20426907.distg2)
	e2:SetOperation(c20426907.disop)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_DISABLE)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTarget(c20426907.distg3)
	e4:SetOperation(c20426907.disop)
	e4:SetLabel(2)
	c:RegisterEffect(e4)
	--tograve
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e5:SetCode(EVENT_ADJUST)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCondition(c20426907.sdcon)
	e5:SetOperation(c20426907.sdop)
	c:RegisterEffect(e5)
end
function c20426907.filter(c)
	return c:IsFaceup() and c:IsLevelAbove(5) and not c:IsType(TYPE_NORMAL)
end
function c20426907.distg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SUMMON_SUCCESS,true)
	if not res then
		res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_FLIP_SUMMON_SUCCESS,true)
	end
	if res then
		local tc=teg:GetFirst()
		if tc:IsLevelBelow(4) and not tc:IsType(TYPE_NORMAL)
			and Duel.SelectYesNo(tp,aux.Stringid(20426907,0)) then
			e:SetLabel(1)
			Duel.SetTargetCard(teg)
			Duel.SetOperationInfo(0,CATEGORY_DISABLE,teg,1,0,0)
		end
		return
	end
	res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS,true)
	if res then
		local g=teg:Filter(c20426907.filter,nil)
		if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(20426907,0)) then
			e:SetLabel(2)
			Duel.SetTargetCard(g)
			Duel.SetOperationInfo(0,CATEGORY_DISABLE,g,g:GetCount(),0,0)
		end
	end
end
function c20426907.distg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:GetFirst():IsLevelBelow(4) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
end
function c20426907.filter3(c)
	return c:IsFaceup() and c:IsLevelAbove(5)
end
function c20426907.distg3(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c20426907.filter3,1,nil) end
	local g=eg:Filter(c20426907.filter3,nil)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,g,g:GetCount(),0,0)
end
function c20426907.disop(e,tp,eg,ep,ev,re,r,rp)
	if c20426907.sdcon(e,tp,eg,ep,ev,re,r,rp) then return end
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	local tc=g:GetFirst()
	while tc do
		Duel.NegateRelatedChain(tc,RESET_TURN_SET)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE_EFFECT)
		e2:SetValue(RESET_TURN_SET)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e2)
		if e:GetLabel()==2 then
			local e3=Effect.CreateEffect(e:GetHandler())
			e3:SetType(EFFECT_TYPE_SINGLE)
			e3:SetCode(EFFECT_LEAVE_FIELD_REDIRECT)
			e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e3:SetReset(RESET_EVENT+0x47e0000)
			e3:SetValue(LOCATION_REMOVED)
			tc:RegisterEffect(e3)
		end
		tc=g:GetNext()
	end
end
function c20426907.sdfilter(c)
	return c:IsFaceup() and c:IsSetCard(0xaa) and not c:IsCode(20426907)
end
function c20426907.sdcon(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c20426907.sdfilter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c20426907.sdop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
end
