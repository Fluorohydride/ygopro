--虚栄の大猿
function c96914272.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c96914272.spcon)
	e1:SetOperation(c96914272.spop)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--lv change
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(96914272,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c96914272.lvcon)
	e2:SetOperation(c96914272.lvop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c96914272.spfilter(c)
	return c:IsRace(RACE_BEAST) and c:IsAbleToGraveAsCost()
end
function c96914272.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c96914272.spfilter,tp,LOCATION_HAND,0,1,c)
end
function c96914272.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c96914272.spfilter,tp,LOCATION_HAND,0,1,1,c)
	Duel.SendtoGrave(g,REASON_COST)
	e:SetLabel(g:GetFirst():GetLevel())
end
function c96914272.lvcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+1
end
function c96914272.lvop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local lv=e:GetLabelObject():GetLabel()
		local clv=c:GetLevel()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		if lv<clv then
			if Duel.SelectOption(tp,aux.Stringid(96914272,1),aux.Stringid(96914272,2))==0 then
				e1:SetValue(lv)
			else e1:SetValue(-lv) end
		else
			Duel.SelectOption(tp,aux.Stringid(96914272,1))
			e1:SetValue(lv)
		end
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
