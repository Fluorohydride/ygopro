--トゥーン・キングダム
function c43175858.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c43175858.target)
	e1:SetOperation(c43175858.activate)
	c:RegisterEffect(e1)
	--change code
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CHANGE_CODE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_FZONE)
	e2:SetValue(15259703)
	c:RegisterEffect(e2)
	--cannot be target
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e3:SetRange(LOCATION_FZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetTarget(aux.TargetBoolFunction(Card.IsType,TYPE_TOON))
	e3:SetValue(c43175858.tgval)
	c:RegisterEffect(e3)
	--destroy replace
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EFFECT_DESTROY_REPLACE)
	e4:SetRange(LOCATION_FZONE)
	e4:SetTarget(c43175858.reptg)
	e4:SetValue(c43175858.repval)
	c:RegisterEffect(e4)
	local g=Group.CreateGroup()
	g:KeepAlive()
	e4:SetLabelObject(g)
end
function c43175858.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ct=Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)
	if ct>3 then ct=3 end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,ct,tp,LOCATION_DECK)
end
function c43175858.activate(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local ct=Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)
	if ct==0 then return end
	if ct>3 then ct=3 end
	local g=Duel.GetDecktopGroup(tp,ct)
	Duel.DisableShuffleCheck()
	Duel.Remove(g,POS_FACEDOWN,REASON_EFFECT)
end
function c43175858.tgval(e,re,rp)
	return rp~=e:GetHandlerPlayer()
end
function c43175858.repfilter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:IsLocation(LOCATION_MZONE)
		and c:IsType(TYPE_TOON) and c:IsReason(REASON_BATTLE+REASON_EFFECT)
end
function c43175858.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c43175858.repfilter,1,nil,tp) and Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0 end
	local ct1=Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)
	if Duel.SelectYesNo(tp,aux.Stringid(43175858,0)) then
		local g=eg:Filter(c43175858.repfilter,nil,tp)
		local cg=nil
		if g:GetCount()==1 then
			cg=g
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESREPLACE)
			cg=g:Select(tp,1,ct1,nil)
		end
		e:GetLabelObject():Clear()
		e:GetLabelObject():Merge(cg)
		local dg=Duel.GetDecktopGroup(tp,cg:GetCount())
		Duel.DisableShuffleCheck()
		Duel.Remove(dg,POS_FACEDOWN,REASON_EFFECT)
		return true
	else return false end
end
function c43175858.repval(e,c)
	local g=e:GetLabelObject()
	return g:IsContains(c)
end
