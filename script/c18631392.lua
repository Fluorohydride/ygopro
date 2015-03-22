--マアト
function c18631392.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c18631392.spcon)
	e2:SetOperation(c18631392.spop)
	c:RegisterEffect(e2)
	--announce 3 cards
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(18631392,0))
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_TOGRAVE+CATEGORY_ATKCHANGE)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetTarget(c18631392.anctg)
	c:RegisterEffect(e3)
end
function c18631392.spfilter(c,rac)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsRace(rac) and c:IsAbleToGraveAsCost()
end
function c18631392.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-2
		and Duel.IsExistingMatchingCard(c18631392.spfilter,tp,LOCATION_MZONE,0,1,nil,RACE_FAIRY)
		and Duel.IsExistingMatchingCard(c18631392.spfilter,tp,LOCATION_MZONE,0,1,nil,RACE_DRAGON)
end
function c18631392.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g1=Duel.SelectMatchingCard(tp,c18631392.spfilter,tp,LOCATION_MZONE,0,1,1,nil,RACE_FAIRY)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g2=Duel.SelectMatchingCard(tp,c18631392.spfilter,tp,LOCATION_MZONE,0,1,1,nil,RACE_DRAGON)
	g1:Merge(g2)
	Duel.SendtoGrave(g1,REASON_COST)
end
function c18631392.anctg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(tp,3) end
	Duel.Hint(HINT_SELECTMSG,tp,0)
	local ac1=Duel.AnnounceCard(tp)
	Duel.Hint(HINT_SELECTMSG,tp,0)
	local ac2=Duel.AnnounceCard(tp)
	Duel.Hint(HINT_SELECTMSG,tp,0)
	local ac3=Duel.AnnounceCard(tp)
	e:SetOperation(c18631392.retop(ac1,ac2,ac3))
end
function c18631392.hfilter(c,code1,code2,code3)
	local code=c:GetCode()
	return (code==code1 or code==code2 or code==code3) and c:IsAbleToHand()
end
function c18631392.retop(code1,code2,code3)
	return
		function (e,tp,eg,ep,ev,re,r,rp)
			if not Duel.IsPlayerCanDiscardDeck(tp,3) then return end
			local c=e:GetHandler()
			Duel.ConfirmDecktop(tp,3)
			local g=Duel.GetDecktopGroup(tp,3)
			local hg=g:Filter(c18631392.hfilter,nil,code1,code2,code3)
			g:Sub(hg)
			if hg:GetCount()~=0 then
				Duel.DisableShuffleCheck()
				Duel.SendtoHand(hg,nil,REASON_EFFECT)
				Duel.ConfirmCards(1-tp,hg)
				Duel.ShuffleHand(tp)
			end
			if g:GetCount()~=0 then
				Duel.DisableShuffleCheck()
				Duel.SendtoGrave(g,REASON_EFFECT+REASON_REVEAL)
			end
			if c:IsRelateToEffect(e) then
				local e1=Effect.CreateEffect(c)
				e1:SetType(EFFECT_TYPE_SINGLE)
				e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
				e1:SetCode(EFFECT_SET_ATTACK)
				e1:SetValue(hg:GetCount()*1000)
				e1:SetReset(RESET_EVENT+0x1ff0000)
				c:RegisterEffect(e1)
				local e2=e1:Clone()
				e2:SetCode(EFFECT_SET_DEFENCE)
				c:RegisterEffect(e2)
			end
		end
end
