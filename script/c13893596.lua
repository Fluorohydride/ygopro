--究極封印神エクゾディオス
function c13893596.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c13893596.spcon)
	e2:SetOperation(c13893596.spop)
	c:RegisterEffect(e2)
	--to grave
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(13893596,0))
	e3:SetCategory(CATEGORY_TOGRAVE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetTarget(c13893596.tgtg)
	e3:SetOperation(c13893596.tgop)
	c:RegisterEffect(e3)
	--atkup
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCode(EFFECT_UPDATE_ATTACK)
	e4:SetValue(c13893596.atkval)
	c:RegisterEffect(e4)
	--redirect
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_LEAVE_FIELD_REDIRECT)
	e5:SetCondition(c13893596.recon)
	e5:SetValue(LOCATION_REMOVED)
	c:RegisterEffect(e5)
end
function c13893596.cfilter(c)
	return not c:IsAbleToDeckOrExtraAsCost()
end
function c13893596.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_GRAVE,0,nil,TYPE_MONSTER)
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and g:GetCount()>0
		and not g:IsExists(c13893596.cfilter,1,nil)
end
function c13893596.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_GRAVE,0,nil,TYPE_MONSTER)
	Duel.SendtoDeck(g,nil,2,REASON_COST)
end
function c13893596.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c13893596.tgfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToGrave()
end
function c13893596.filter(c,rc,code)
	return c:IsRelateToCard(rc) and c:GetCode()==code
end
function c13893596.tgop(e,tp,eg,ep,ev,re,r,rp)
	local WIN_REASON_EXODIUS = 0x14
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local c=e:GetHandler()
	local g=Duel.SelectMatchingCard(tp,c13893596.tgfilter,tp,LOCATION_HAND+LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	if tc and Duel.SendtoGrave(tc,REASON_EFFECT)~=0 and tc:IsLocation(LOCATION_GRAVE)
		and c:IsRelateToEffect(e) and c:IsFaceup() then
		tc:CreateRelation(c,RESET_EVENT+0x1fe0000)
		local a1=false local a2=false local a3=false local a4=false local a5=false 
		local g=Duel.GetFieldGroup(tp,LOCATION_GRAVE,0)
		local gc=g:GetFirst()
		while gc do
			if c13893596.filter(gc,c,8124921) then a1=true
			elseif c13893596.filter(gc,c,44519536) then a2=true
			elseif c13893596.filter(gc,c,70903634) then a3=true
			elseif c13893596.filter(gc,c,7902349) then a4=true
			elseif c13893596.filter(gc,c,33396948) then a5=true
			end
			gc=g:GetNext()
		end
		if a1 and a2 and a3 and a4 and a5 then
			Duel.Win(tp,WIN_REASON_EXODIUS)
		end
	end
end
function c13893596.atkval(e,c)
	return Duel.GetMatchingGroupCount(Card.IsType,c:GetControler(),LOCATION_GRAVE,0,nil,TYPE_NORMAL)*1000
end
function c13893596.recon(e)
	return e:GetHandler():IsFaceup()
end
